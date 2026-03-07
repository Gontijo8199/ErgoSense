import firebase_admin
from firebase_admin import credentials, db
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.animation import FuncAnimation
import numpy as np
import threading

SERVICE_ACCOUNT = "py/chave.json" 
DB_URL = "https://ergosense-teste-default-rtdb.firebaseio.com"
MASTER_SENSOR = '0x45' # Sensor que dita o ritmo da atualização do dashboard


SENSOR_MAP = {
    '0x45': {'pos': (0, 0), 'type': 'heatmap', 'path': '/devices/sensors/l5cx/0x45'},
    '0x46': {'pos': (0, 1), 'type': 'heatmap', 'path': '/devices/sensors/l5cx/0x46'},
    '0x47': {'pos': (1, 0), 'type': 'circle',  'path': '/devices/sensors/l4cd/0x47'},
    '0x48': {'pos': (1, 1), 'type': 'circle',  'path': '/devices/sensors/l4cd/0x48'},
    '0x49': {'pos': (2, 0), 'type': 'circle',  'path': '/devices/sensors/l4cd/0x49'},
    '0x50': {'pos': (2, 1), 'type': 'circle',  'path': '/devices/sensors/l4cd/0x50'},
}

MAX_DIST_MM = 1500

class SensorDashboard:
    def __init__(self, sensor_map):
        self.sensor_map = sensor_map
        self.data_lock = threading.Lock() 

        self.sync_buffer = {}  
        self.current_sync_ts = 0 
        
        self.latest_data = {}
        for sensor_id, info in sensor_map.items():
            if info['type'] == 'heatmap':
                self.latest_data[sensor_id] = np.zeros((8, 8))
            else:
                self.latest_data[sensor_id] = 0.0

        self._init_firebase()
        self._setup_plot()

    def _init_firebase(self):
        """Inicializa a conexão e os listeners do Firebase."""
        if not firebase_admin._apps:
            cred = credentials.Certificate(SERVICE_ACCOUNT)
            firebase_admin.initialize_app(cred, {"databaseURL": DB_URL})
        
        print("Ligado ao Firebase. A iniciar listeners...")
        
        for sensor_id, info in self.sensor_map.items():
            ref = db.reference(info['path'])
            ref.listen(lambda event, sid=sensor_id: self._firebase_callback(event, sid))

    def _extract_matrix(self, data):
        """Procura a matriz dentro da estrutura complexa do JSON."""
        if isinstance(data, list) and len(data) == 64:
            return data
        if isinstance(data, dict):
            if 'matrix' in data and isinstance(data['matrix'], list):
                return data['matrix']
            keys = sorted(data.keys())
            if keys:
                return self._extract_matrix(data[keys[-1]])
        return None

    def _firebase_callback(self, event, sensor_id):
        """Callback otimizado para lidar com diferenças de tempo entre sensores."""
        if event.data is None:
            return

        try:
            raw_data = event.data
            path_info = event.path  
            ts = None
            content = None

            clean_path = path_info.strip('/')
            if clean_path.isdigit():
                ts = clean_path
                content = raw_data
            elif isinstance(raw_data, dict):
                keys = [k for k in raw_data.keys() if str(k).isdigit()]
                if keys:
                    ts = sorted(keys)[-1]
                    content = raw_data[ts]
                elif 'timestamp' in raw_data:
                    ts = str(raw_data['timestamp'])
                    content = raw_data

            if ts is not None:
                try:
                    ts_int = int(ts)
                except ValueError:
                    return

                val = None
                if self.sensor_map[sensor_id]['type'] == 'heatmap':
                    val = self._extract_matrix(content)
                else:
                    val = content.get('distance_mm') if isinstance(content, dict) else content

                if val is not None:
                    with self.data_lock:
                        if ts_int not in self.sync_buffer:
                            self.sync_buffer[ts_int] = {}
                        
                        self.sync_buffer[ts_int][sensor_id] = val
                        
                        if sensor_id == MASTER_SENSOR:
                            if self.current_sync_ts == 0 or ts_int > self.current_sync_ts:
                                self.current_sync_ts = ts_int
                            
        except Exception as e:
            print(f"Erro no callback de {sensor_id}: {e}")

    def update_frame(self, frame):
        with self.data_lock:
            self.fig_title.set_text(f"TS Sincronizada: {self.current_sync_ts}")
            
            if self.current_sync_ts != 0:
                sorted_ts = sorted(self.sync_buffer.keys())
                
                for t in sorted_ts:
                    if t <= self.current_sync_ts:
                        for sensor_id, val in self.sync_buffer[t].items():
                            self.latest_data[sensor_id] = val
                    else:
                        break 
                
                old_keys = [t for t in sorted_ts if t < self.current_sync_ts]
                for k in old_keys:
                    self.sync_buffer.pop(k)
                
                if len(self.sync_buffer) > 50:
                    keys_to_remove = sorted(self.sync_buffer.keys())[:-30]
                    for k in keys_to_remove:
                        self.sync_buffer.pop(k)

        for sensor_id, info in self.sensor_map.items():
            data = self.latest_data[sensor_id]
            plot_info = self.plots[sensor_id]

            if info['type'] == 'heatmap':
                try:
                    matrix = np.array(data).reshape(8, 8)
                    
                    matrix = np.rot90(matrix, k=2)
                    
                    plot_info['obj'].set_array(matrix)
                except:
                    pass
            elif info['type'] == 'circle':
                dist = float(data)
                radius = min(100, max(2, (dist / MAX_DIST_MM) * 100))
                plot_info['obj'].set_radius(radius)
                plot_info['obj'].set_color('#ff4444' if dist >= 150 else '#00ff44')
                plot_info['text'].set_text(f"{int(dist)} mm")

        return []

    def _setup_plot(self):
        """Configura a janela e os subplots do Matplotlib."""
        plt.style.use('dark_background') 
        self.fig = plt.figure(figsize=(10, 12))
        
        self.fig_title = self.fig.suptitle(f"Ergosense Dashboard (Sync: {MASTER_SENSOR})", color='yellow')

        gs = gridspec.GridSpec(3, 2, height_ratios=[1, 1, 1])
        self.plots = {}

        for sensor_id, info in self.sensor_map.items():
            row, col = info['pos']
            ax = self.fig.add_subplot(gs[row, col])
            ax.set_title(f"Sensor {sensor_id} ({info['type'].upper()})", fontsize=10, color='white')
            
            if info['type'] == 'heatmap':
                im = ax.imshow(np.zeros((8, 8)), cmap='inferno', interpolation='nearest', vmin=0.0, vmax=2000.0)
                self.fig.colorbar(im, ax=ax)
                ax.axis('off')
                self.plots[sensor_id] = {'type': 'heatmap', 'obj': im, 'ax': ax}
                
            elif info['type'] == 'circle':
                ax.set_xlim(-120, 120)
                ax.set_ylim(-120, 120)
                ax.set_aspect('equal')
                ax.axis('off')
                
                outline = plt.Circle((0, 0), 100, color='#333333', fill=False)
                ax.add_patch(outline)
                circle = plt.Circle((0, 0), 2, color='#888888', alpha=0.8)
                ax.add_patch(circle)
                text = ax.text(0, -140, "0 mm", ha='center', color='white', fontsize=12)
                
                self.plots[sensor_id] = {'type': 'circle', 'obj': circle, 'text': text, 'ax': ax}

        plt.tight_layout(rect=[0, 0.03, 1, 0.95])

    def run(self):
        anim = FuncAnimation(self.fig, self.update_frame, interval=200, cache_frame_data=False)
        plt.show()

if __name__ == "__main__":
    try:
        dashboard = SensorDashboard(SENSOR_MAP)
        dashboard.run()
    except KeyboardInterrupt:
        print("\nDashboard encerrado.")
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.animation import FuncAnimation
import numpy as np

CSV_L4 = "dataset_l4cd.csv"
CSV_L5 = "dataset_l5cx.csv"

MAX_DIST_MM = 1500

SENSOR_MAP = {
    '0x45': {'pos': (0, 0), 'type': 'heatmap'},
    '0x46': {'pos': (0, 1), 'type': 'heatmap'},
    '0x47': {'pos': (1, 0), 'type': 'circle'},
    '0x48': {'pos': (1, 1), 'type': 'circle'},
    '0x49': {'pos': (2, 0), 'type': 'circle'},
    '0x50': {'pos': (2, 1), 'type': 'circle'},
}


class SensorReplayDashboard:

    def __init__(self):

        print("Carregando dataset...")

        self.timeline = {}
        self._load_data()

        self.timestamps = sorted(self.timeline.keys())
        self.pointer = 0

        print(f"Timestamps carregados: {len(self.timestamps)}")

        self.latest_data = {}

        for sid, info in SENSOR_MAP.items():
            if info['type'] == 'heatmap':
                self.latest_data[sid] = np.zeros((8, 8))
            else:
                self.latest_data[sid] = 0

        self._setup_plot()



    def _load_data(self):


        df4 = pd.read_csv(
            CSV_L4,
            header=None,
            sep=",",
            names=["timestamp", "addr", "distance_mm"]
        )

        for row in df4.itertuples(index=False):

            ts = int(row.timestamp)
            addr = hex(int(row.addr))

            if ts not in self.timeline:
                self.timeline[ts] = {}

            self.timeline[ts][addr] = row.distance_mm


        pixel_cols = [f"p{i}" for i in range(64)]

        df5 = pd.read_csv(
            CSV_L5,
            header=None,
            sep=",",
            names=["timestamp", "addr"] + pixel_cols
        )

        for row in df5.itertuples(index=False):

            ts = int(row.timestamp)
            addr = hex(int(row.addr))

            pixels = list(row[2:66])

            if ts not in self.timeline:
                self.timeline[ts] = {}

            self.timeline[ts][addr] = pixels



    def update_frame(self, frame):

        if self.pointer >= len(self.timestamps):
            return []

        ts = self.timestamps[self.pointer]

        self.fig_title.set_text(f"Timestamp: {ts}")

        frame_data = self.timeline[ts]

        for sid, val in frame_data.items():
            self.latest_data[sid] = val

        self.pointer += 1

        for sid, info in SENSOR_MAP.items():

            data = self.latest_data[sid]
            plot = self.plots[sid]

            if info['type'] == 'heatmap':

                try:

                    matrix = np.array(data).reshape(8, 8)

                    matrix = np.rot90(matrix, 2)

                    plot["obj"].set_array(matrix)

                except:
                    pass

            elif info['type'] == 'circle':

                dist = float(data)

                radius = min(100, max(2, (dist / MAX_DIST_MM) * 100))

                plot["obj"].set_radius(radius)

                if dist < 150:
                    plot["obj"].set_color("#ff4444")
                else:
                    plot["obj"].set_color("#00ff44")

                plot["text"].set_text(f"{int(dist)} mm")

        return []


    def _setup_plot(self):

        plt.style.use("dark_background")

        self.fig = plt.figure(figsize=(10, 12))

        self.fig_title = self.fig.suptitle(
            "Ergosense Dataset Replay",
            color="yellow"
        )

        gs = gridspec.GridSpec(3, 2)

        self.plots = {}

        for sid, info in SENSOR_MAP.items():

            r, c = info["pos"]

            ax = self.fig.add_subplot(gs[r, c])

            ax.set_title(f"Sensor {sid}", fontsize=10)

            if info["type"] == "heatmap":

                im = ax.imshow(
                    np.zeros((8, 8)),
                    cmap="inferno",
                    interpolation="nearest",
                    vmin=0,
                    vmax=2000
                )

                self.fig.colorbar(im, ax=ax)

                ax.axis("off")

                self.plots[sid] = {"obj": im, "ax": ax}

            else:

                ax.set_xlim(-120, 120)
                ax.set_ylim(-120, 120)
                ax.set_aspect("equal")
                ax.axis("off")

                outline = plt.Circle((0, 0), 100, color="#333333", fill=False)
                ax.add_patch(outline)

                circle = plt.Circle((0, 0), 2, color="#888888", alpha=0.8)
                ax.add_patch(circle)

                text = ax.text(
                    0,
                    -140,
                    "0 mm",
                    ha="center",
                    color="white",
                    fontsize=12
                )

                self.plots[sid] = {
                    "obj": circle,
                    "text": text,
                    "ax": ax
                }

        plt.tight_layout(rect=[0, 0.03, 1, 0.95])

    def run(self):

        print("Iniciando replay...")

        anim = FuncAnimation(
            self.fig,
            self.update_frame,
            interval=200,
            cache_frame_data=False
        )

        plt.show()

if __name__ == "__main__":

    try:

        dashboard = SensorReplayDashboard()

        dashboard.run()

    except KeyboardInterrupt:

        print("\nDashboard encerrado.")
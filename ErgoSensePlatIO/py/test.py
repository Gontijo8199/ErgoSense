import firebase_admin
from firebase_admin import credentials, db
import matplotlib.pyplot as plt

SERVICE_ACCOUNT = "py/chave.json"
DB_URL = "https://ergosense-teste-default-rtdb.firebaseio.com"
PATH = "/devices/sensors/l4cd/0x50"
MAX_MM = 2000

cred = credentials.Certificate(SERVICE_ACCOUNT)
firebase_admin.initialize_app(cred, {
    "databaseURL": DB_URL
})

ref = db.reference(PATH)

plt.ion()
fig, ax = plt.subplots()

circle = plt.Circle((0, 0), 10)
ax.add_patch(circle)

ax.set_xlim(-120, 120)
ax.set_ylim(-120, 120)
ax.set_aspect("equal")

def update_circle(distance):
    radius = max(2, (distance / MAX_MM) * 100)
    circle.set_radius(radius)
    fig.canvas.draw_idle()
    fig.canvas.flush_events()

def listener(event):
    # ignorar snapshot inicial
    if event.path == "/":
        return

    data = event.data
    if not data:
        return

    distance = data.get("distance_mm", 0)
    timestamp = event.path.strip("/")

    print(f"timestamp: {timestamp} | distância: {distance} mm")
    update_circle(distance)

ref.listen(listener)

plt.show(block=True)

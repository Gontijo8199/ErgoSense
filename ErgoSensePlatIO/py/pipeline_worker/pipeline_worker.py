
import firebase_admin
from firebase_admin import credentials, db
import pandas as pd
import time



cred = credentials.Certificate("../chave.json")

firebase_admin.initialize_app(cred, {
    "databaseURL": "https://ergosense-teste-default-rtdb.firebaseio.com"
})


l4_ref = db.reference("devices/ergosense0/l4cd_batch")
l5_ref = db.reference("devices/ergosense0/l5cx_batch")


def export_l4cd() -> None:

    data = l4_ref.get()

    if not data:
        return

    rows = []

    for key, batch in data.items():

        ts = batch["timestamp"]

        for item in batch["data"]:

            rows.append({
                "timestamp": ts,
                "addr": item["addr"],
                "distance_mm": item["dist"]
            })

    df = pd.DataFrame(rows)

    df.to_csv("dataset_l4cd.csv", mode="a", header=False, index=False)

    l4_ref.delete()

    print("L4CD exportado:", len(rows))

def export_l5cx() -> None:

    data = l5_ref.get()

    if not data:
        return

    rows = []

    for key, batch in data.items():

        ts = batch["timestamp"]

        for frame in batch["frames"]:

            row = {
                "timestamp": ts,
                "addr": frame["addr"]
            }

            for i, pixel in enumerate(frame["pixels"]):
                row[f"p{i}"] = pixel

            rows.append(row)

    df = pd.DataFrame(rows)

    df.to_csv("dataset_l5cx.csv", mode="a", header=False, index=False)

    l5_ref.delete()

    print("L5CX exportado:", len(rows))

if __name__ == "__main__":
    while True:

        try:

            export_l4cd()
            export_l5cx()

        except Exception as e:

            print("Erro:", e)

        time.sleep(30)


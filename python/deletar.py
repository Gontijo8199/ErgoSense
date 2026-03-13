import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

cred = credentials.Certificate('chave.json')
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://ergosense-teste-default-rtdb.firebaseio.com'
})

caminho_do_no = 'devices'
ref = db.reference(caminho_do_no)

ref.delete()

print(f"Nó {caminho_do_no} deletado com sucesso do Realtime Database!")
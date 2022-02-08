# subscriber
from hbmqtt.mqtt.constants import QOS_1
import paho.mqtt.client as mqtt
import sqlite3

client = mqtt.Client()

client.connect('***.***.**.*', 1885)

def on_connect(client, userdata, flags, rc):
    if   rc==0:
        print("Connect successful")
    elif rc==1:
        print("incorrect protocol version")
    elif rc==2:
        print("invalid client identifier")
    elif rc==3:
        print("server unavailable")
    elif rc==4:
        print("bad username or password")
        # print (client,flags)
    elif rc==5:
        print("not authorised")
    elif 6<rc<255:
        print("Currently unused")
    else:
        print("Connect failed")
    print("Connected !")
    client.subscribe("jumlahBarang1")
    client.subscribe("jumlahBarang2")
    client.subscribe("jumlahBarang3")
    client.subscribe("jumlahBarang4")
    

def on_message(client, userdata, message):
    msg = message.payload.decode()
    if msg !="rusak":
        msg_list = msg.split(":")
        
        conn = sqlite3.connect("web_app/market/stockSUMO.db")
        cur = conn.cursor()
        cur.execute("SELECT prod_weight FROM stock WHERE prod_name = '{}'".format(msg_list[0]))
        temp = cur.fetchone()
        berat = temp[0]

        stock_hitungan = round(float(msg_list[1])/float(berat))
        cur.execute("UPDATE jumlahBDT SET unit = '{}' WHERE nama_barang = '{}'".format(stock_hitungan,msg_list[0]))

        conn.commit()
        conn.close()

while True:
    client.on_connect = on_connect
    client.on_message = on_message

    client.loop_forever()


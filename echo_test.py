"""
sends some data, and shows received data.

Alan Marchiori
Bucknell University
"""
import logging
logging.basicConfig(level=logging.INFO)

from xbeechat import XbeeChat
import time
import struct

def pkt(p, data):
    try:
        if 'id' in data and data['id'] == 'rx':
            logging.info ("RX: rssi: {}, src_addr: {}, data: {}".format(
                struct.unpack("B", data['rssi'])[0],
                struct.unpack(">H", data['source_addr'])[0],
                data['rf_data']))
    except Exception as x:
        logging.error("error: " + str(x))

def main():
    x= None
    try:
        x = XbeeChat('/dev/ttyUSB0', channel = 11,
                     panid=0x1001, address=1515,
                     callback = pkt)

        res = {}

        count = 0
        while (True):
            m = x.send(1616, "hello {}\0".format(count))
            m.wait(2)
            count += 1
            if m.status not in res:
                res[m.status] = 0
            res[m.status] += 1

            time.sleep(1)

            #print (res)
            if count >= 1000:
                break

    except Exception as err:
        logging.error(err)

    finally:
        if x:
            x.close()

if __name__=="__main__":
    main()


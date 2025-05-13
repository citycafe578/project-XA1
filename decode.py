import pygeohash as pgh

def decode(data, time_mode):
    if(time_mode == 0): #滾轉 俯仰 航向 衛星數量 通訊品質 座標
        data = data.split(",")
        list_data = data.split(" ")
        for i in range(len(list_data - 1)):
            list_data[i] = int(list_data[i])

        if(list_data[5] == "INVALID"):
            list_data[5] = 0
            list_data[6] = 0
        else:
            lat, lon = pgh.decode(list_data[5])
            list_data[5] = lat
            list_data[6] = lon
        return list_data
    else: #時間 滾轉 俯仰 航向 衛星數量 通訊品質 座標
        data = data.split(",")
        list_data = data.split(" ")
        for i in range(1, len(list_data - 1)):
            list_data[i] = int(list_data[i])

        if(list_data[5] == "INVALID"):
            list_data[6] = 0
            list_data[7] = 0
        else:
            lat, lon = pgh.decode(list_data[5])
            list_data[6] = lat
            list_data[7] = lon
        return list_data
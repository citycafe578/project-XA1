import pygeohash as pgh

def decode(data):
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
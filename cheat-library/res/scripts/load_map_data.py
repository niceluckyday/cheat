import requests
import json
import re

# Reading api
with open("map_api.txt", "r") as map_api_file:
    api_list = []
    for line in map_api_file.readlines():
        name, address = line.split()
        api_list.append((name, address))

# Getting data
message_list = []
for (name, address) in api_list:
    r = requests.get(address)
    if r.status_code != 200:
        print(f"Failed load data for {name}")
        continue

    message_list.append((name, json.loads(r.content.decode())))

# Refactor data to our format
refactored_data_list = []
for (name, message) in message_list:
    label_list = message["data"]["label_list"]
    point_list = message["data"]["point_list"]

    label_map = {}
    for label in label_list:
        label_map[label["id"]] = label

    data = {}
    for point in point_list:
        label_id = point["label_id"]
        if label_id not in data:
            label = label_map[label_id]
            clear_name = re.sub(r"[^\w\d_]", "", label["name"])
            data[label_id] = {
                "name": label["name"],
                "clear_name": clear_name,
                "points": [],
                "icon": label["icon"]
            }

        points = data[label_id]["points"]
        points.append({
            "id": point["id"],
            "x_pos": point["x_pos"],
            "y_pos": point["y_pos"]
        })

    refactored_data_list.append((name, data))

# Writing refactored data to json files
for (name, data) in refactored_data_list:
    with open(f"../{name}.json", "w") as out_json_file:
        json.dump(data, out_json_file, indent=4)

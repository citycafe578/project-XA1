document.addEventListener("DOMContentLoaded", function(){
    var recordPage = document.getElementById("record_page");
    var closeBtn = document.getElementById("close");
    var fileSelect = document.getElementById("file-select");
    var slider = document.getElementById("slider");
    var timeContent = document.getElementById("time-content");
    var fileContent = [];
    var map;

    document.getElementById("record").addEventListener("click", function() {
        recordPage.showModal();
        setTimeout(initializeMap, 100);
    });

    closeBtn.addEventListener("click", function() {
        recordPage.close();
    });

    function initializeMap() {
        if (!map) {
            map = L.map('float_map').setView([51.505, -0.09], 13);
            L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
                maxZoom: 19,
                attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
            }).addTo(map);
            map.locate({
                setView: true,
                watch: true,
                maxZoom: 18,
                enableHighAccuracy: true,
                timeout: 10000
            });
        }
    }

    window.updateSliderValue = function(value) {
        document.getElementById("slider-value").textContent = value;
        if (fileContent.length > 0) {
            var lineIndex = Math.floor(fileContent.length * (value / 100));
            var content = fileContent[lineIndex] || "無內容";
            timeContent.textContent = content;
            console.log("Slider value:", value, "Line index:", lineIndex, "Content:", content);
        }
    };

    fileSelect.addEventListener("change", function() {
        var selectedFile = fileSelect.value;
        fetch("/get_file_content", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({ file_name: selectedFile })
        })
        .then(response => response.json())
        .then(data => {
            if (data && data.length > 0) {
                fileContent = [];
                data.forEach(rawData => {
                    fetch("/decode_data", {
                        method: "POST",
                        headers: {
                            "Content-Type": "application/json"
                        },
                        body: JSON.stringify({ data: rawData, time_mode: 0 })
                    })
                    .then(response => response.json())
                    .then(decodedResponse => {
                        if (decodedResponse.success) {
                            console.log("Decoded Data:", decodedResponse.decoded_data);
                            fileContent.push(decodedResponse.decoded_data);
                            updateSliderValue(slider.value);
                            document.getElementById('time-content').textContent = `${data.data[0]}`;
                            document.getElementById('pitch_content').textContent = `${data.data[1]}`;
                            document.getElementById('sat_num_content').textContent = `${data.data[4]}`;
                            document.getElementById('sat_quality_content').textContent = `${data.data[5]}`;
                            document.getElementById('longitude_content').textContent = `${data.data[6]}`;
                            document.getElementById('latitude_content').textContent = `${data.data[7]}`;

                        } else {
                            console.error("Error decoding data:", decodedResponse.error);
                        }
                    })
                    .catch(error => console.error("Error fetching decoded data:", error));
                });
            } else {
                console.error("No data found in file.");
            }
        })
        .catch(error => console.error("Error fetching file content:", error));
    });

    var recording = document.getElementById("recording");
    var temp = false;
    recording.addEventListener("click", function(){
        if (!temp) {
            this.classList.add("recording");
            this.innerText = "紀錄中";
            temp = true;
            fetch('/record')
                .then(response => response.json())
                .then(data => {
                    console.log('Success:', data);
                })
                .catch(error => {
                    console.error('Error:', error);
                });
        } else {
            this.innerText = "開始記錄";
            this.classList.remove("recording");
            temp = false;
            fetch('/stop_record')
                .then(response => response.json())
                .then(data => {
                    console.log('Success:', data);
                    updateFileList();
                })
                .catch(error => {
                    console.error('Error:', error);
                });
        }
    });

    function updateFileList() {
        fetch('/get_file')
            .then(response => response.json())
            .then(files => {
                fileSelect.innerHTML = '';
                if (files.length > 0) {
                    files.forEach(file => {
                        var option = document.createElement('option');
                        option.value = file;
                        option.textContent = file;
                        fileSelect.appendChild(option);
                    });
                } else {
                    var option = document.createElement('option');
                    option.value = '';
                    option.textContent = '沒有檔案';
                    option.disabled = true;
                    fileSelect.appendChild(option);
                }
            })
            .catch(error => console.error("Error fetching file list:", error));
    }

    updateFileList();

    function fetchQueueData() {
        fetch('/get_queue_data')
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    console.log("Queue Data:", data.data);
                    const depression_img = document.getElementById('depression', 'speed');
                    depression_img.style.transform = `translateY(${data.data[0]}px`;
                    const roll_img = document.getElementById('roll');
                    roll_img.style.transform = `rotate(${data.data[1]}deg)`;
                    const course_img = document.getElementById('course_photo');
                    roll_img.style.transform = `rotate(${data.data[2]}deg)`;
                    document.getElementById('sat_num').textContent = `${data.data[3]}`;
                    document.getElementById('sat_quality').textContent = `${data.data[4]}`;

                    const latitude = 0
                    const longitude = 0
                    if(data.data[5] == 0 && data.data[5] == 0){
                        document.getElementById('sat_mode').textContent = `${Disable}`;
                    }else{
                        document.getElementById('sat_mode').textContent = `${Enable}`;
                        latitude = data.data[5];
                        longitude = data.data[6];
                    }
                    if (map) {
                        map.setView([latitude, longitude], 13);
                    }
                } else {
                    console.log("Queue is empty");
                }
            })
            .catch(error => console.error("Error fetching queue data:", error));
    }

    setInterval(fetchQueueData, 200);
    initializeMap();
});

const record = document.getElementById("record");
const recordpage = document.getElementById("record_page");
const close = document.getElementById("close")
record.addEventListener("click", function() {
    console.log("go into record page");
    recordpage.showModal();
});

close.addEventListener("click", function(){
    recordpage.close();
})

function startTime(){
    var today = new Date();
    var hh = today.getHours();
    var mm = today.getMinutes();
    var ss = today.getSeconds();
    mm = checkTime(mm);
    ss = checkTime(ss);
    document.getElementById('clock').innerText = hh + ":" + mm + ":" + ss;
    var timeoutId = setTimeout(startTime, 500);
}

function checkTime(i){
    if(i < 10) {
        i = "0" + i;
    }
    return i;
}

function fetchQueueData() {
    fetch('/get_queue_data')
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                console.log("Queue Data:", data.data);
            } else {
                console.log("Queue is empty");
            }
        })
        .catch(error => console.error("Error fetching queue data:", error));
}
setInterval(fetchQueueData, 2000);
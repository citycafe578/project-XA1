// const webcam = document.getElementById("webcam")
// const light = document.getElementById("light")
// const FPS = document.getElementById("FPS")
// const gray_scale = document.getElementById("gray_scale")
// const image = document.getElementById("stream"); 
// const imageUrl = "{{ image_url }}";
// webcam.addEventListener("change", function() {
//     if (this.checked) {
//         image.src = "http://192.168.0.106:81/stream";
//     } else {
//         image.src = imageUrl;
//     }
// });

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
            fileContent = data;
            updateSliderValue(slider.value);
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
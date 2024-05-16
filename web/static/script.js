const webcam = document.getElementById("webcam")
// const light = document.getElementById("light")
// const FPS = document.getElementById("FPS")
// const gray_scale = document.getElementById("gray_scale")
const image = document.getElementById("stream"); 
const imageUrl = "{{ image_url }}";
webcam.addEventListener("change", function() {
    if (this.checked) {
        image.src = "http://192.168.0.106:81/stream";
    } else {
        image.src = imageUrl;
    }
});

var recording = document.getElementById("recording");
var temp = false;
recording.addEventListener("click", function(){
    if(!temp){
        this.classList.add("recording");
        this.innerText = "紀錄中";
        temp = true;
    }else{
        this.innerText = "開始記錄";
        this.classList.remove("recording");
        temp = false;
    }
})

const record = document.getElementById("record");
const recordpage = document.getElementById("record_page");
record.addEventListener("click", function() {
    console.log("go into record page");
    recordpage.style.display = "block";
});


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
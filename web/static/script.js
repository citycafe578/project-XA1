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
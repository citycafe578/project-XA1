// document.addEventListener("DOMContentLoaded", function() {
//     let now_site = 0;
    
//     function updateSite() {
//         const speedImage = document.getElementById("speed");
//         if (now_site === 50) {
//             now_site = -1;
//         } else if (now_site === -50) {
//             now_site = 1;
//         } else {
//             now_site += 10;
//         }
//         speedImage.style.transform = `translate(0px, ${now_site}px)`;
//     }

//     setInterval(updateSite, 100);
// });

//從這邊開始就是chat gpt 給的
// document.addEventListener("DOMContentLoaded", function() {
//     const videoElement = document.getElementById('videoElement');

//     // 获取用户媒体，启动摄像头
//     navigator.mediaDevices.getUserMedia({ video: { url: 'http://192.168.0.106/' } })
//     .then(function(stream) {
//         videoElement.srcObject = stream;
//     })
//     .catch(function(error) {
//         console.error('Error accessing the webcam:', error);
//     });
// });
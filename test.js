// 獲取彈出視窗和關閉按鈕的元素
var record = document.getElementById("record");
var recordPage = document.getElementById("record_page");
var close = document.getElementsByClassName("close")[0];

// 當用戶點擊記錄按鈕時顯示彈出視窗
record.addEventListener("click", function() {
  console.log("go into record page");
  recordPage.style.display = "block";
});

// 當用戶點擊關閉按鈕時關閉彈出視窗
close.addEventListener("click", function() {
  recordPage.style.display = "none";
});

// 當用戶點擊彈出視窗以外的地方時關閉彈出視窗
window.addEventListener("click", function(event) {
  if (event.target == recordPage) {
    recordPage.style.display = "none";
  }
});

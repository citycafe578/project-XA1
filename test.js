fetch('http://192.168.0.106:81/')
    .then((response) => {
        console.log(response); 
    })
    .catch((error) => {
        console.log(`Error: ${error}`);
    })
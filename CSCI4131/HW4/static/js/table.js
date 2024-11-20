let interval_id = undefined;
let tableLength = document.querySelectorAll(".date");

let savedDate = [];
for (i = 0; i < tableLength.length; i++) { 
    savedDate[i] = tableLength[i].innerText;
}   

interval_id = setInterval(updateTime, 1000);

function updateTime() {
    for (i = 0; i < tableLength.length; i++) {
        let rowDate = Date.parse(savedDate[i]);

        let currentTime = new Date().getTime();

        let timeDiff = rowDate - currentTime;
        
        let days = Math.floor(timeDiff / (1000 * 60 * 60 * 24));
        let hours = Math.floor((timeDiff % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
        let minutes = Math.floor((timeDiff % (1000 * 60 * 60)) / (1000 * 60));
        let seconds = Math.floor((timeDiff % (1000 * 60)) / 1000);

        if (timeDiff < 0) {
            tableLength[i].innerText = "Auction Ended"
        } else {
            tableLength[i].innerText = days + "days "+ hours + "hours " + minutes + "minutes " + seconds + "seconds";
        }
    }
} 

let rowLength = document.querySelectorAll(".tableRow");
let previewImg = document.getElementById("previewImg");
let previewDesc = document.getElementById("previewDesc");
let buttonDelete = document.querySelectorAll(".delete");


for (let i = 0; i < rowLength.length; i++) {
    rowLength[i].addEventListener("mouseover", function() {
        console.log("Got into here!");
        previewImg.innerHTML = '<img id="imgPreview" src="' + rowLength[i].getAttribute("data-image") + '">';
        previewDesc.innerText = "This is the " + rowLength[i].getAttribute("data-description") + " preview listing";
    });
};

for (let i = 0 ; i < buttonDelete.length; i++) {
    buttonDelete[i].addEventListener("click", () => {
        fetch("/api/delete_listing", { 
            method: 'DELETE',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                    id: parseInt(buttonDelete[i].closest("tr").getAttribute('data-listing-id'))
                }),
            }
        );
        rowLength[i].remove();
    });
};
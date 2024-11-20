let hiddenButton = document.getElementById("bidButton");
let hiddenText = document.getElementById("hidden")

hiddenButton.addEventListener("click", function() { 
    if (hiddenText.classList.contains("none")) {
        hiddenText.classList.replace("none", "display");
        hiddenButton.innerText = "Cancel Bid";
    } else {
        hiddenText.classList.replace("display", "none");
        hiddenButton.innerText = "Place Bid";
    }
});
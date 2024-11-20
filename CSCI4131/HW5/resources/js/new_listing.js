let categorySelected = document.getElementById("category");
let hiddenText = document.getElementById("hidden")


 categorySelected.addEventListener("change", function() { 
    if (this.value == "Other") {
        hiddenText.classList.replace("none", "display");
    } else {
        hiddenText.classList.replace("display", "none");
    }
});
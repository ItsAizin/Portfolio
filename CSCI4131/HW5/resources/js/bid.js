let hiddenButton = document.getElementById("bidButton");
let hidden = document.querySelector(".none");
let bidSubmit = document.getElementById("bidsubmit");
const bidName = document.getElementById("name");
const bidAmount = document.getElementById("amount");
const bidComment = document.getElementById("desc");
const id = document.getElementById("id");
const topbid = document.getElementById("topbid");
const date = document.getElementById("date");
const numbids = document.getElementById("numbids");
const category = document.getElementById("category");

hiddenButton.addEventListener("click", function() { 
    if (hidden.classList.contains("none")) {
        hidden.classList.replace("none", "display");
        hiddenButton.innerText = "Cancel Bid";
    } else {
        hidden.classList.replace("display", "none");
        hiddenButton.innerText = "Place Bid";
    }
});

bidSubmit.addEventListener("click", async function() {
    const response = await fetch("/api/place_bid", {
        credentials: "include",
        method: "POST",
        headers: {
            "Content-Type": "application/json; charset=UTF-8",
        },
        body: JSON.stringify({
            id: id.value, 
            name: bidName.value, 
            amount: bidAmount.value, 
            comment: bidComment.value, 
            topbid: topbid.value, 
            date: date.value, 
            numbids: numbids.value, 
            category: category.value
        }),
    });

    const status = response.status;

    if (status == 400 || status == 500) {
        alert("Server Error");
    } else if (status == 409) {
        bidAmount.style.borderColor = "#e43d12";
        bidAmount.style.transition = "ease-in-out all .4s";
    } else { 
        bidAmount.value = ""
        bidComment.value = ""
        hidden.classList.replace("display", "none");
        hiddenButton.innerText = "Place Bid";

        document.querySelectorAll(".bid").forEach(el => el.remove());
        let bid_list = document.getElementById("bid_list");
        let messages = await response.json();

        console.log(messages)
        let bid_data =  messages[0]['bids']
        console.log(bid_data)
        
        for (let i = 0; i <bid_data.length; i++) { 
            const the_div = document.createElement("div");
            the_div.classList.add("bid")
            const bidder = document.createElement("span");
            bidder.classList.add("bidder");
            const amount = document.createElement("span");
            amount.classList.add("amount");
            const the_para = document.createElement("p");

            the_div.appendChild(bidder);
            the_div.appendChild(amount);
            the_div.appendChild(the_para);

            bidder.innerText = bid_data[i].name;
            amount.innerText = parseFloat(bid_data[i].amount);
            the_para.innerText = bid_data[i].comment;
            bid_list.appendChild(the_div);
        }
    }
});
const express = require('express');
const session = require('express-session');
const app = express();
const port = 4131

app.set("views", "templates")
app.set("view engine", "pug")
const cookies = require('cookie-parser')

app.use(express.urlencoded({ extended: true}))
app.use(express.json())
app.use(cookies())
app.use(session({secret: 'aslkdfjasl;dfkjasldkfjalsdkfjasdf'}))

app.use(express.static('resources'))

let rateLimitStore = [];
const RATE_LIMIT = 3;
const RATE_LIMIT_WINDOW = 10;
listings = [{'title' : 'NES', 'link' : "/listing/1", 'image' : '/images/NES.jpg', 'desc': "This is the Nintendo Entertainment System (NES). It is the highest of quality! The item has already sold off to the highest bidder."
    , 'numbids' : '3','category' : 'Consoles', 'id' : '1', 'topbid' : 22.21, 'date' : "2024-10-22", 'bids' : [{'name' : 'Jessie', 'amount' : 22.21, 'comment' : ''}, {'name' : 'James', 'amount' : 12.02, 'comment' : ''}, {'name' : 'Meowth', 'amount' : 17.72, 'comment' : "That's the name! I want this neowth!"}] },
    {'title' : 'Pokemon', 'link' : "/listing/2",'image' : '/images/PokemonYellow.jpg', 'desc': "This is the Pokemon Yellow. It is the highest of quality! The item has already sold off to the highest bidder."
    ,'numbids' : '3','category' : 'Video Games', 'id' : '2', 'topbid' : 22.21,'date' : "2024-10-22", 'bids' : [{'name' : 'Charizard', 'amount' : 22.21, 'comment' : ''}, {'name' : 'Pikachu', 'amount' : 12.02, 'comment' : "Pika Pika!"}, {'name' : 'Bulbasaur', 'amount' : 17.72, 'comment' : "" }]},
    {'title' : 'Pacman', 'link' : "/listing/3", 'image' : '/images/Pacman.jpg', 'desc': "This is a Pacman Cup. It is the highest of quality! The item has already sold off to the highest bidder."
    ,'numbids' : '3','category' : 'Merch', 'id' : '3', 'topbid' : 22.21, 'date' : "2024-11-01", 'bids' : [{'name' : 'Pacman', 'amount' : 22.21, 'comment' : ''}, {'name' : 'Cherry', 'amount' : 12.02, 'comment' : "Please don't eat me!"}, {'name' : 'Ghost', 'amount' : 17.72, 'comment' : "" }]}]

const checkRateLimit = () => {
    const now = new Date();
    rateLimitStore = rateLimitStore.filter(time =>
    (now - time) <= RATE_LIMIT_WINDOW * 1000
    );

    if (rateLimitStore.length >= RATE_LIMIT) {
    const oldestRequest = rateLimitStore[0];
    const retryAfter = RATE_LIMIT_WINDOW - ((now - oldestRequest) / 1000);
    return { passed: false, retryAfter };
    }

    rateLimitStore.push(now);
    return { passed: true };
};

app.get('/', (req, res) => {
    res.status(200).render('mainpage.pug');
})

app.get('/main', (req, res) => {
    res.status(200).render('mainpage.pug');
})

app.get('/gallery', (req, res) => {
    let category = req.query.category;
    let query = req.query.query;
    let renderListings = [];
    let listingsDictionary = {}

    for (listing of listings) {
        if (listing['topbid'] == 0) {
            listingsDictionary = {
                image: listing['image'],
                title: listing['title'],
                id: listing['id'],
                numbids: listing['numbids'],
                category: listing['category'],
                topbid: "No Bid",
                date: listing['date'],
                link: listing['link']
            }
            renderListings.push(listingsDictionary)
        } else if ((category === "All Categories" && query.length === 0) || 
        (category === "All Categories" && listing.title.includes(query)) || 
        (listing.category.includes(category ) && query.length === 0) || 
        (listing.category.includes(category) && listing.title.includes(query)) ||
        (category == undefined && query == undefined)) {
            listingsDictionary = {
                image: listing['image'],
                title: listing['title'],
                id: listing['id'],
                numbids: listing['numbids'],
                category: listing['category'],
                topbid: listing['topbid'],
                date: listing['date'],
                link: listing['link']
            }
            renderListings.push(listingsDictionary)
        }
    }

    res.status(200).render('gallery.pug', {renderListings:renderListings});
})

app.get('/listing/:id', (req, res) => {
    console.log(req.cookies)
    const id = req.params.id;
    for (listing of listings) {
        if (id == listing['id']) {
            let listingsDictionary = {}
            
            if (JSON.stringify(req.cookies) == '{}') {
                listingsDictionary = {
                    id: listing.id,
                    topbid: listing.topbid,
                    date: listing.date,
                    numbids: listing.numbids,
                    category: listing.category,
                    title: listing.title,
                    image: listing.image,
                    desc: listing.desc,
                    cookies: true,
                    cookies_name: req.cookies.biddername,
                    bids: listing.bids,
                }
            } else {
                listingsDictionary = {
                    id: listing.id,
                    topbid: listing.topbid,
                    date: listing.date,
                    numbids: listing.numbids,
                    category: listing.category,
                    title: listing.title,
                    image: listing.image,
                    desc: listing.desc,
                    cookies: false,
                    cookies_name: 'Your Name',
                    bids: listing.bids,
                }
            }
            res.status(200).render('listing.pug', {val:listingsDictionary});        
        }
    }
})

app.get('/create', (req, res) => {
    res.status(200).render('create.pug');
})

app.get('/creation_success', (req, res) => {
    res.status(200).render('create_success.pug');
})

app.get('/create_fail', (req, res) => {
    res.status(200).render('create_fail.pug');
})

app.post('/create', (req, res) => {
    const saleDate = new Date(req.body.date)
    const todayDate = new Date()

    if ((req.body.category == 'other' && req.body.hidden == '') ||
    (req.body.title == '') || (req.body.image == '') || (req.body.desc == '') || (saleDate < todayDate)) {
        res.status(400).render('create_fail.pug')
    } else {
        let otherCategory = undefined
        if (req.body.category == 'other' && req.body.hidden != '') {
            otherCategory = req.body.hidden
        } else {
            otherCategory = req.body.category
        }

        let newId = listings.length + 1

        let newListingDict = {
            id: newId.toString(),
            title: req.body.title,
            link: "/listing/" + newId.toString(),
            image: req.body.image,
            numbids: "0",
            topbid: "0",
            bids: [],
            category: otherCategory,
            date: req.body.date
        }

        listings.push(newListingDict)
        res.status(200).render('gallery.pug', {renderListings: listings})
    }
})

app.post('/api/place_bid', (req, res) => {
    if (rateLimitStore == undefined) {
        res.status(429).set('Retry-After', rate).send('')
    }

    if ((req.body == '') || (req.get('Content-Type') == '') || (req.get('Content-Type') != 'application/json; charset=UTF-8') ||
    (!(req.body instanceof Object)) || (typeof req.body.name != 'string')  || (typeof parseFloat(req.body.amount) != 'number') ||
    (req.body.name == '') || (!('name' in req.body)) || (!('amount' in req.body)) || (!('comment' in req.body)) 
    ) {
        console.log("If statement")
        res.status(400).set('Content-Type', 'application/json').send('')
    } else if (parseFloat(req.body.amount) < req.body.topbid) {
        console.log("Else if statement")
        res.status(409).set('Content-Type', 'application/json').send(listings)
    } else {
        let cookies = req.cookies.biddername
        const amount = parseFloat(req.body.amount).toFixed(2)
        if (cookies == undefined) { 
            cookies = req.body.name
        }

        let newBidDict = {
            name: cookies,
            amount: amount,
            comment: req.body.comment
        }

        listings[parseInt(req.body.id) - 1]['bids'].push(newBidDict)
        listings[parseInt(req.body.id) - 1]['numbids']++
        listings[parseInt(req.body.id) - 1]['topbid'] = amount


        console.log(amount)
        res.status(201).set({'Content-Type': 'application/json','Set-Cookie': req.body.name }).send(listings)
    }
})

app.delete('/api/delete_listing', (req, res) => {
    const rate = checkRateLimit();
    if (rateLimitStore == undefined) {
        res.status(429).set('Retry-After', rate).send('')
    }

    if ((req.get('Content-Type') != 'application/json') || 
    (req.get('Content-Type') != 'application/json') || (
        req.body == undefined) ||
    (!("id" in req.body)) || 
    (!(req.body instanceof Object))) {
        res.status(400).set('Content-Type', 'application/json').send('')
    } else if (req.body != '' && req.get('Content-Type')  == 'application/json') {
        if (!(listings.some(e => e.id == req.body.id))) {
            res.status(404).render('404.pug')
        } else {
            for (listing of listings) {
                if (req.body.id.toString() == listing['id']) {
                    listings.splice((listings.indexOf(listing)), 1)
                }
            }
        }
    }

    res.status(204).set('Content-Type', "application/json; charset=utf-8").send('')
})

app.get('resources/css/main.css', (req,res) => {
    res.status(200).set('Content-Type','text/css').send('http://localhost:4131/css/main.css')
})

app.get('resources/js/bid.js', (req,res) => {
    res.status(200).set('Content-Type','text/javascript').send('http://localhost:4131/js/bid.js')
})
app.get('resources/js/table.js', (req,res) => {
    res.status(200).set('Content-Type','text/javascript').send('http://localhost:4131/js/table.js')
})
app.get('resources/js/new_listing.js', (req,res) => {
    res.status(200).set('Content-Type','text/javascript').send('http://localhost:4131/js/new_listing.js')
})

app.get('resources/images/NES.jpg', (req,res) => {
    res.status(200).set('Content-Type', 'image/jpeg').send('http://localhost:4131/images/NES.jpg')
})

app.get('resources/images/Pacman.jpg', (req,res) => {
    res.status(200).set('Content-Type', 'image/jpeg').send('http://localhost:4131/images/Pacman.jpg')
})

app.get('resources/images/PokemonYellow.jpg', (req,res) => {
    res.status(200).set('Content-Type', 'image/jpeg').send('http://localhost:4131/images/PokemonYellow.jpg')
})

app.get('resources/images/VideoGames.jpg', (req,res) => {
    res.status(200).set('Content-Type', 'image/jpeg').send('http://localhost:4131/images/VideoGames.jpg')
})

app.use(function(req,res){
    res.status(404).render('404.pug');
});

app.listen(port, () => {
    console.log(`Server running on port ${port}`);
})
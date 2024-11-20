from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib
from datetime import date, datetime

# PUT YOUR GLOBAL VARIABLES AND HELPER FUNCTIONS HERE.
# It is not required, but is is strongly recommended that you write a function to parse form data out of the URL, and a second function for generating the contact log page html.

listings = [{'title' : 'NES', 'link' : "/listing/1", 'image' : '/images/NES', 'desc': "This is the Nintendo Entertainment System (NES). It is the highest of quality! The item has already sold off to the highest bidder."
            , 'numbids' : '3','category' : 'Consoles', 'id' : '1', 'topbid' : 22.21, 'date' : "2024-10-22", 'bids' : [{'name' : 'Jessie', 'amount' : 22.21, 'comment' : ''}, {'name' : 'James', 'amount' : 12.02, 'comment' : ''}, {'name' : 'Meowth', 'amount' : 17.72, 'comment' : "That's the name! I want this neowth!"}] },
            {'title' : 'Pokemon', 'link' : "/listing/2",'image' : '/images/PokemonYellow', 'desc': "This is the Pokemon Yellow. It is the highest of quality! The item has already sold off to the highest bidder."
            ,'numbids' : '3','category' : 'Video Games', 'id' : '2', 'topbid' : 22.21,'date' : "2024-10-22", 'bids' : [{'name' : 'Charizard', 'amount' : 22.21, 'comment' : ''}, {'name' : 'Pikachu', 'amount' : 12.02, 'comment' : "Pika Pika!"}, {'name' : 'Bulbasaur', 'amount' : 17.72, 'comment' : "" }]},
            {'title' : 'Pacman', 'link' : "/listing/3", 'image' : '/images/Pacman', 'desc': "This is a Pacman Cup. It is the highest of quality! The item has already sold off to the highest bidder."
            ,'numbids' : '3','category' : 'Merch', 'id' : '3', 'topbid' : 22.21, 'date' : "2024-11-01", 'bids' : [{'name' : 'Pacman', 'amount' : 22.21, 'comment' : ''}, {'name' : 'Cherry', 'amount' : 12.02, 'comment' : "Please don't eat me!"}, {'name' : 'Ghost', 'amount' : 17.72, 'comment' : "" }]}
            ]

def append_gallery(elemParam):
    if elemParam['topbid'] == "0":
        appended_data = f"""
            <tr data-image="{elemParam['image']}" data-description="{elemParam['title']}" class="tableRow">
                <td><a href={elemParam['link']} class="listingtitle">{elemParam['title']}</a></td>
                <td>{elemParam['numbids']}</td>
                <td>{escape_html(elemParam['category'])}</td>
                <td>No Bid</td>
                <td>{elemParam['date']}</td>
                <td class="date">{elemParam['date']}</td>
            </tr>
            """
    else:
        appended_data = f"""
                    <tr data-image="{elemParam['image']}" data-description="{elemParam['title']}" class="tableRow">
                        <td><a href={elemParam['link']} class="listingtitle">{elemParam['title']}</a></td>
                        <td>{elemParam['numbids']}</td>
                        <td>{elemParam['category']}</td>
                        <td>{typeset_dollars(elemParam['topbid'])}</td>
                        <td>{elemParam['date']}</td>
                        <td class="date">{elemParam['date']}</td>
                    </tr>
                    """
    
    return appended_data


def escape_html(str):
    str = str.replace("&", "&amp;")
    str = str.replace('"', "&quot;")
    str = str.replace('>', "&gt;")
    str = str.replace('<', "&lt;")
    str = str.replace('+', "&nbsp;")


    # you need more.

    return str


def unescape_url(url_str):
    import urllib.parse

    # NOTE -- this is the only place urllib is allowed on this assignment.
    return urllib.parse.unquote_plus(url_str)


def parse_query_parameters(response):
        print(response)
        if not ("query" and "category") in response[0]:
            return {'query' : '', 'category' : 'All Categories'}
            
        if '&' in response[0]:
            # Split the query and parameter
            splitQ, *splitP = response[0].split("&")
            # print(splitQ)
            # print(splitP)
            parsedParam = {}

            # Split the queries key and value
            keyQ, valueQ = splitQ.split("=")

            parsedParam[keyQ] = unescape_url(valueQ)

            # Split the paramters key and value
            keyP, valueP = splitP[0].split("=")

            # Appending the query and parameters key-value pairs in the dictionary
            parsedParam[keyP] = unescape_url(valueP)

            return parsedParam
        else:
            # print("Something went wrong")
            return {'query' : 'not', 'category' : 'found'}
def render_listing(listing):
    # Get everything past listing
    
    listing_id = listing[9:]
    data = ""

    # Iterate each element in listings and see if listing_id is found in listings
    # If so, return the page. Else return 404 page
    for i in listings:
        if listing_id == i['id']:
            dynamicListing1 = f"""<!DOCTYPE html>
                <html lang="en">

                <head>
                    <meta charset="UTF-8">
                    <title>Video Game Auction - {i['title']}</title>
                    <link rel="stylesheet" href="/main.css">
                    <script async defer src="/js/bid.js"></script>
                </head>

                <body>
                    <nav>
                        <a href="/">Home</a>
                        <a href="/gallery">Listings</a>
                        <a href="/create">Add Listing</a>
                            <form action="/gallery" method="get">
                                <input type="search" name="query" class="search">
                                <select name="category" class="category">
                                <option value="All Categories">All Categories</option>
                                <option value="Consoles">Consoles</option>
                                <option value="Video Games">Video Games</option>
                                <option value="Merch">Merch</option>
                                </select>
                                <input type="submit" value="Search" class="submit">
                            </form>
                    </nav>
                        <h1>{i['title']}</h1>

                <div class="fadeInUp-animation">
                    <div class="flexbox">
                        <div class="desc">
                            <img src="{i['image']}" alt="Picture of {i['title']}" class="console">
                            <h2>{i['desc']}</h2>
                        </div>
                
                        <div class="bids">
                            <div class="newBid">
                                <h3>Bids</h3>
                                <button id="bidButton">Place Bid</button>
                            </div>
                            
                            <div id="hidden" class="none">
                                <form action="/place_bid" method="post">
                                    <div class="bidname">
                                        <label for="name">Your Name:</label>
                                        <input type="text" name="name" required>
                                    </div>
                                    
                                    <div class="bidamount">
                                        <label for="amount">Amount:</label>
                                        <input type="number" name="amount" min="5" step="0.01" required>
                                    </div>
                                
                                    <div class="bidcomment">
                                        <label for="desc">Comment:</label>
                                        <textarea name="desc" rows="5" cols="40"></textarea>
                                    </div>
                                        <input type="hidden" name="id" value="{i['id']}">
                                                                            
                                        <input type="hidden" name="topbid" value="{i['topbid']}">
                                        
                                        <input type="hidden" name="date" value="{i['date']}">
                                        
                                        <input type="hidden" name="numbids" value="{i['numbids']}">
                                        
                                        <input type="hidden" name="category" value="{i['category']}">

                                    <input type="submit" value="Submit" class="bidsubmit">
                                </form>
                            </div>
                """
                
            dynamicListing2 = f"""
                            </div>
                        </div>    
                    </div>
                </body>
            </html>
            
            """
            if i['bids'] is None:
                return dynamicListing1 + dynamicListing2
            else:
                for x in range(len(i['bids'])):
                    data = data + f"""
                        <div class="bid">
                            <span class="bidder">{i['bids'][x]['name']}</span>
                            <span class="amount">${i['bids'][x]['amount']}</span>
                            <p>{i['bids'][x]['comment']}</p>
                        </div>
                    """

            return dynamicListing1 + data + dynamicListing2
        
    return "/404.html"


def render_gallery(query, category):          
    dyanmicListings1 = """
        <!DOCTYPE html>
        <html lang="en">

        <head>
            <meta charset="UTF-8">
            <title>Video Game Auction - Listings</title>
            <link rel="stylesheet" href="/main.css">
            <script async defer src="/js/table.js"></script>
        </head>

        <body>
            <nav>
                <a href="/">Home</a>
                <a href="/gallery">Listings</a>
                <a href="/create">Add Listing</a>
                    <form action="/gallery" method="get">
                        <input type="search" name="query" class="search">
                        <select name="category" class="category">
                        <option value="All Categories">All Categories</option>
                        <option value="Consoles">Consoles</option>
                        <option value="Video Games">Video Games</option>
                        <option value="Merch">Merch</option>
                        </select>
                        <input type="submit" value="Search" class="submit">
                    </form>
            </nav>
                <h1>Gallery</h1>
                <div class="fadeInUp-animation">
                    <div class="tableview">
                        <table class="tableGallery">
                        <thead>
                            <tr>
                                <th>Number of Lists</th>
                                <th>Number of Bids</th>
                                <th>Category</th>
                                <th>Top Bid</th>
                                <th>Sale Date</th>
                                <th>Time Left</th>        
                            </tr>
                        </thead>
                        <tbody>
        """
    
    dyanmicListings2 = """ 
                        </tbody>
                    </table>
                    <div class="preview">
                        <div id="previewImg"></div>
                        <p id="previewDesc"></p>
                    </div>     
                </div>   
            </body>
        </html>
        """

    counter = 0
    data = ""
    # Go through each element in listings array and see if the key of the elements
    # dictionary matches to list that page.
    for elem in listings:
        if category == "All Categories" and len(query) == 0:
            counter =  counter + 1
            data = data + append_gallery(elem)
        elif category == "All Categories" and query in elem['title']:
            counter =  counter + 1
            data = data + append_gallery(elem)
        elif category in elem['category'] and len(query) == 0:
            counter =  counter + 1
            data = data + append_gallery(elem)
        elif category in elem['category'] and query in elem['title']:
            counter =  counter + 1
            data = data + append_gallery(elem)

    if counter == 0:
        return "/404"
    else:
        return dyanmicListings1 + data + dyanmicListings2

def add_new_listing(params):
    tmp = {}
    categorySave = ""
    for i in range(len(params)):
        key, value = params[i].split("=")

        # Key is irrelavant but want to know if there is a value for every key     
        if value is None or key == "category" and value == "Other" and key == "hidden" and value is None or key == "date" and datetime.strptime(value, "%Y-%m-%d").strftime("%Y-%m-%d") < datetime.now().strftime("%Y-%m-%d"):
            return False
        # If hidden value isn't empty, then change the category to hidden
        elif key == "hidden" and value is not None:
            tmp['category'] = value 
        else:
            tmp[key] = unescape_url(value)
            if key == "category":
                categorySave = value

        
    # Getting max id and creating a key-value for the tmp dictionary        
    curr_max_id = int(listings[-1]['id'])
    tmp['id'] = str(curr_max_id + 1)
    tmp['link'] = "/listing/" + tmp['id']
    tmp['numbids'] = "0"
    tmp['topbid'] = "0"
    tmp['bids'] = []
    tmp['category'] = categorySave
    
    # print(tmp['image'])
    
    print("Before appending")
    print(tmp)
    listings.append(tmp)
    # print(listings)
    
    f = open("static/html/dynamiclistings.html", "w")
    f.write(render_gallery("", "All Categories"))
    f.close()
    
    return True

def add_new_bid(params):
    tmp = {}
    bidContent = {}
    
    for i in range(len(params)):
        key, value = params[i].split("=")
        tmp[key] = value
    
    # print(float(tmp['amount']))
    
    tmp['numbids'] = int(tmp['numbids']) + 1
    bidContent['name'] = escape_html(tmp['name'])
    bidContent['amount'] = float(tmp['amount'])
    bidContent['comment'] = escape_html(tmp['desc'])
    
    for elem in listings:
        if elem['id'] == tmp['id']:
            if float(tmp['amount']) < float(elem['topbid']) or tmp['name'] is None or tmp['amount'] is None or tmp['date'] == 0:
                return False  
            elem['bids'].append(bidContent)
            curr_max_id = int(listings[-1]['id'])
            galleryId = "/gallery/" + str(elem['id'])
            elem['numbids'] = tmp['numbids']
            elem['topbid'] = float(tmp['amount'])
            # elem.update({'numbids' : tmp['numbids'], 'topbid' : float(tmp['amount'])})
            
            f = open("static/html/dynamiclistings.html", "w")
            f.write(render_gallery("", "All Categories"))
            f.close()
            
            f = open("static/html/dynamiclisting.html", "w")
            f.write(render_listing(galleryId))
            f.close()
    return True


# Provided function -- converts numbers like 42 or 7.347 to "$42.00" or "$7.35"
def typeset_dollars(number):
    return f"${number:.2f}"

def server_GET(url: str) -> tuple[str | bytes, str, int]:
    """
    url is a *PARTIAL* URL. If the browser requests `http://localhost:4131/contact?name=joe`
    then the `url` parameter will have the value "/contact?name=joe". (so the schema and
    authority will not be included, but the full path, any query, and any anchor will be included)

    This function is called each time another program/computer makes a request to this website.
    The URL represents the requested file.

    This function should return three values (string or bytes, string, int) in a list or tuple. The first is the content to return
    The second is the content-type. The third is the HTTP Status Code for the response
    """
    url, *parameters = url.split("?")

    if url == "/" or url == "/main":
        return (open("static/html/mainpage.html").read(), "text/html", 200)
    elif url == "/gallery":
        # print(listings)
        
        if parameters:
            print(parameters[0])
            galleryParam = parse_query_parameters(parameters)

            if galleryParam['query'] == 'not' and galleryParam['category'] == 'found':
                return (open("static/html/404.html").read(), "text/html", 404)
            else:
                render_gallary_result = render_gallery(galleryParam['query'], galleryParam['category'])

                if render_gallary_result == "/404":
                    return (open("static/html/404.html").read(), "text/html", 404)
                else:    
                    f = open("static/html/dynamiclistings.html", "w")
                    f.write(render_gallary_result)
                    f.close()

                    return (open("static/html/dynamiclistings.html").read(), "text/html", 200)
        else:
            return (open("static/html/dynamiclistings.html").read(), "text/html", 200)
    elif url.startswith("/listing"):

        f = open("static/html/dynamiclisting.html", "w")
        f.write(render_listing(url))
        f.close()

        return (open("static/html/dynamiclisting.html").read(), "text/html", 200)
    if url == "/create":
        return (open("static/html/create.html").read(), "text/html", 200)
    elif url == "/create_fail":
        return (open("static/html/create_fail.html").read(), "text/html", 200)
    elif url == "/create_success":
        return (open("static/html/create_success.html").read(), "text/html", 200)
    elif url == "/images/main":
        return (open("static/images/VideoGames.jpg", "rb").read(), "image/jpeg", 200)
    elif url == "/images/NES":
        return (open("static/images/NES.jpg", "rb").read(), "image/jpeg", 200)
    elif url == "/images/PokemonYellow":
        return (open("static/images/PokemonYellow.jpg", "rb").read(), "image/jpeg", 200)
    elif url == "/images/Pacman":
        return (open("static/images/Pacman.jpg", "rb").read(), "image/jpeg", 200)
    elif url == "/main.css":
        return (open("static/css/main.css").read(), "text/css", 200)
    elif url == "/js/bid.js":
        return (open("static/js/bid.js").read(), "text/javascript", 200)
    elif url == "/js/new_listing.js":
        return (open("static/js/new_listing.js").read(), "text/javascript", 200)
    elif url == "/js/table.js":
        return (open("static/js/table.js").read(), "text/javascript", 200)
    else:
        return (open("static/html/404.html").read(), "text/html", 404)   

            

def server_POST(url: str, body: str) -> tuple[str | bytes, str, int]:
    """
    url is a *PARTIAL* URL. If the browser requests `http://localhost:4131/contact?name=joe`
    then the `url` parameter will have the value "/contact?name=joe". (so the schema and
    authority will not be included, but the full path, any query, and any anchor will be included)

    This function is called each time another program/computer makes a POST request to this website.

    This function should return three values (string or bytes, string, int) in a list or tuple. The first is the content to return
    The second is the content-type. The third is the HTTP Status Code for the response
    """
    body = body.split("&")
    
    print(body)
    
    if url == "/create":
        listingBool = add_new_listing(body)
        if listingBool:
            f = open("static/html/dynamiclisting.html", "w")
            f.write(render_listing(url))
            f.close()
        
            return (open("static/html/dynamiclistings.html").read(), "text/html", 201)
        else:
            return (open("static/html/dynamiclistings.html").read(), "text/html", 400)
    if url == "/place_bid":
        bidBool = add_new_bid(body)
        if bidBool:
            return (open("static/html/dynamiclisting.html").read(), "text/html", 201)
        else:
            return (open("static/html/create_fail.html").read(), "text/html", 400)

        
# You shouldn't need to change content below this. It would be best if you just left it alone.

class RequestHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        # Read the content-length header sent by the BROWSER
        content_length = int(self.headers.get('Content-Length',0))
        # read the data being uploaded by the BROWSER
        body = self.rfile.read(content_length)
        # we're making some assumptions here -- but decode to a string.
        body = str(body, encoding="utf-8")

        message, content_type, response_code = server_POST(self.path, body)

        # Convert the return value into a byte string for network transmission
        if type(message) == str:
            message = bytes(message, "utf8")

        # prepare the response object with minimal viable headers.
        self.protocol_version = "HTTP/1.1"
        # Send response code
        self.send_response(response_code)
        # Send headers
        # Note -- this would be binary length, not string length
        self.send_header("Content-Length", len(message))
        self.send_header("Content-Type", content_type)
        self.send_header("X-Content-Type-Options", "nosniff")
        self.end_headers()

        # Send the file.
        self.wfile.write(message)
        return

    def do_GET(self):
        # Call the student-edited server code.
        message, content_type, response_code = server_GET(self.path)

        # Convert the return value into a byte string for network transmission
        if type(message) == str:
            message = bytes(message, "utf8")

        # prepare the response object with minimal viable headers.
        self.protocol_version = "HTTP/1.1"
        # Send response code
        self.send_response(response_code)
        # Send headers
        # Note -- this would be binary length, not string length
        self.send_header("Content-Length", len(message))
        self.send_header("Content-Type", content_type)
        self.send_header("X-Content-Type-Options", "nosniff")
        self.end_headers()

        # Send the file.
        self.wfile.write(message)
        return


def run():
    PORT = 4131
    print(f"Starting server http://localhost:{PORT}/")
    server = ("", PORT)
    httpd = HTTPServer(server, RequestHandler)
    httpd.serve_forever()


run()

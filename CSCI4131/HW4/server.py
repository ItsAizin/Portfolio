from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib  # Only for parse.unquote and parse.unquote_plus.
import json
import base64
from typing import Union, Optional
import re
import datetime
import time
# If you need to add anything above here you should check with course staff first.


listings = [{'title' : 'NES', 'link' : "/listing/1", 'image' : '/images/NES', 'desc': "This is the Nintendo Entertainment System (NES). It is the highest of quality! The item has already sold off to the highest bidder."
            , 'numbids' : '3','category' : 'Consoles', 'id' : '1', 'topbid' : 22.21, 'date' : "2024-10-22", 'bids' : [{'name' : 'Jessie', 'amount' : 22.21, 'comment' : ''}, {'name' : 'James', 'amount' : 12.02, 'comment' : ''}, {'name' : 'Meowth', 'amount' : 17.72, 'comment' : "That's the name! I want this neowth!"}] },
            {'title' : 'Pokemon', 'link' : "/listing/2",'image' : '/images/PokemonYellow', 'desc': "This is the Pokemon Yellow. It is the highest of quality! The item has already sold off to the highest bidder."
            ,'numbids' : '3','category' : 'Video Games', 'id' : '2', 'topbid' : 22.21,'date' : "2024-10-22", 'bids' : [{'name' : 'Charizard', 'amount' : 22.21, 'comment' : ''}, {'name' : 'Pikachu', 'amount' : 12.02, 'comment' : "Pika Pika!"}, {'name' : 'Bulbasaur', 'amount' : 17.72, 'comment' : "" }]},
            {'title' : 'Pacman', 'link' : "/listing/3", 'image' : '/images/Pacman', 'desc': "This is a Pacman Cup. It is the highest of quality! The item has already sold off to the highest bidder."
            ,'numbids' : '3','category' : 'Merch', 'id' : '3', 'topbid' : 22.21, 'date' : "2024-11-01", 'bids' : [{'name' : 'Pacman', 'amount' : 22.21, 'comment' : ''}, {'name' : 'Cherry', 'amount' : 12.02, 'comment' : "Please don't eat me!"}, {'name' : 'Ghost', 'amount' : 17.72, 'comment' : "" }]}
            ]

nav_HTML = """
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
"""

end_dyanmicHTML = """
                </div>
            </body>
        </html>
"""

# Provided helper function. This function can help you implement rate limiting
rate_limit_store = []


def pass_api_rate_limit() -> tuple[bool, int | None]:
    """This function will keep track of rate limiting for you.
    Call it once per request, it will return how much delay would be needed.
    If it returns 0 then process the request as normal
    Otherwise if it returns a positive value, that's the number of seconds
    that need to pass before the next request"""
    from datetime import datetime, timedelta

    global rate_limit_store
    # you may find it useful to change these for testing, such as 1 request for 3 seconds.s
    RATE_LIMIT = 4  # requests per second
    RATE_LIMIT_WINDOW = 10  # seconds
    # Refresh rate_limit_store to only "recent" times
    rate_limit_store = [
        time
        for time in rate_limit_store
        if datetime.now() - time <= timedelta(seconds=RATE_LIMIT_WINDOW)
    ]
    if len(rate_limit_store) >= RATE_LIMIT:
        return (
            RATE_LIMIT_WINDOW - (datetime.now() - rate_limit_store[0]).total_seconds()
        )
    else:
        # Add current time to rate_limit_store
        rate_limit_store.append(datetime.now())
        return 0


def escape_html(str):
    # this i s a bare minimum for hack-prevention.
    # You might want more.
    str = str.replace("&", "&amp;")
    str = str.replace('"', "&quot;")
    str = str.replace("<", "&lt;")
    str = str.replace(">", "&gt;")
    str = str.replace("'", "&#39;")
    str = str.replace('+', "&nbsp;")
    return str


def unescape_url(url_str):
    import urllib.parse

    # NOTE -- this is the only place urllib is allowed on this assignment.
    return urllib.parse.unquote_plus(url_str)


def parse_query_parameters(response):
    if not ("query" and "category") in response[0]:
        return {'query' : '', 'category' : 'All Categories'}
    
    pairs = response.split("&")
    parsed_params = {}

    for pair in pairs:
        key = unescape_url(pair.split("=")[0])
        value = unescape_url(pair.split("=")[1])
        parsed_params[key] = value

    return parsed_params

def render_listing(id, cookies):
    for listing in listings:
        if listing['id'] == id:
            start_renderedHTML = """
                <!DOCTYPE html>
                <html lang="en">

                <head>
                    <meta charset="UTF-8">
                    <title>Video Game Auction</title>
                    <link rel="stylesheet" href="/main.css">
                    <script async defer src="/js/bid.js"></script>
                </head>
            """
            start_renderedHTML = start_renderedHTML + nav_HTML

            start_renderedHTML = start_renderedHTML + f"""
                <h1>{listing['title']}</h1>

                <div class="fadeInUp-animation">
                    <div class="flexbox">
                        <div class="desc">
                            <img src="{listing['image']}" alt="Picture of {listing['title']}" class="console">
                            <h2>{listing['desc']}</h2>
                        </div>
                
                        <div class="bids">
                            <div class="newBid">
                                <h3>Bids</h3>
                                <button id="bidButton">Place Bid</button>
                            </div>
                """
            if len(cookies) == 0:
                start_renderedHTML = start_renderedHTML + f"""
                <div id="hidden" class="none">
                                    <div class="bidname">
                                        <label for="name">Your Name:</label>
                                        <input type="text" name="name" id="name" placeholder="Your Name" required>
                                    </div>
                                    
                                    <div class="bidamount">
                                        <label for="amount">Amount:</label>
                                        <input type="number" name="amount" id="amount" min="5" step="0.01" placeholder="0.00" required>
                                    </div>
                                
                                    <div class="bidcomment">
                                        <label for="desc">Comment:</label>
                                        <textarea name="desc" id="desc" rows="5" cols="40" placeholder="Your comment"></textarea>
                                    </div>
                                        <input type="hidden" name="id" id="id" value="{listing['id']}">
                                                                            
                                        <input type="hidden" name="topbid" id="topbid" value="{listing['topbid']}">
                                        
                                        <input type="hidden" name="date" id="date" value="{listing['date']}">
                                        
                                        <input type="hidden" name="numbids" id="numbids" value="{listing['numbids']}">
                                        
                                        <input type="hidden" name="category" id="category" value="{listing['category']}">

                                        <input type="button" value="Submit" id="bidsubmit">
                            </div>
                            <div id="bid_list">
                """    
            else:
                start_renderedHTML = start_renderedHTML + f"""
                <div id="hidden" class="none">
                                    <div class="bidname">
                                        <label for="name">Your Name:</label>
                                        <input type="text" name="name" id="name" placeholder="{cookies['bidder_name']}" required>
                                    </div>
                                    
                                    <div class="bidamount">
                                        <label for="amount">Amount:</label>
                                        <input type="number" name="amount" id="amount" min="5" step="0.01" placeholder="0.00" required>
                                    </div>
                                
                                    <div class="bidcomment">
                                        <label for="desc">Comment:</label>
                                        <textarea name="desc" id="desc" rows="5" cols="40" placeholder="Your comment"></textarea>
                                    </div>
                                        <input type="hidden" name="id" id="id" value="{listing['id']}">
                                                                            
                                        <input type="hidden" name="topbid" id="topbid" value="{listing['topbid']}">
                                        
                                        <input type="hidden" name="date" id="date" value="{listing['date']}">
                                        
                                        <input type="hidden" name="numbids" id="numbids" value="{listing['numbids']}">
                                        
                                        <input type="hidden" name="category" id="category" value="{listing['category']}">

                                        <input type="button" value="Submit" id="bidsubmit">
                            </div>
                """          

            
            mid_renderedHTML = """"""
            for bid in listing['bids']:
                mid_renderedHTML = mid_renderedHTML + f"""
                    <div class="bid">
                        <span class="bidder">{bid['name']}</span>
                        <span class="amount">${bid['amount']}</span>
                        <p>{bid['comment']}</p>
                    </div>
                """
    
    renderedHTML = start_renderedHTML + mid_renderedHTML + """
                </div>
            </div>
        </div>   
    """
    
    return renderedHTML + end_dyanmicHTML

def render_gallery(query, category):
    renderedHTML = """
        <!DOCTYPE html>
        <html lang="en">

        <head>
            <meta charset="UTF-8">
            <title>Video Game Auction</title>
            <link rel="stylesheet" href="/main.css">
            <script async defer src="/js/table.js"></script>
        </head>
    """
    
    renderedHTML = renderedHTML + nav_HTML
    
    renderedHTML = renderedHTML + """
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
                        <th></th>      
                    </tr>
                </thead>
                <tbody>
    """
    
    # Pass through each listing and first see if there is a top bid or not. Then determine if it fits under one of ecah case.
    for listing in listings:
        if listing['topbid'] == 0:
            renderedHTML = renderedHTML + f"""
                <tr data-image="{listing['image']}" data-description="{listing['title']}" class="tableRow" data-listing-id="{listing['id']}>
                    <td><a href={listing['link']} class="listingtitle">{listing['title']}</a></td>
                    <td>{listing['numbids']}</td>
                    <td>{escape_html(listing['category'])}</td>
                    <td>No Bid</td>
                    <td>{listing['date']}</td>
                    <td class="date">{listing['date']}</td>
                    <td><input type="button" value="Delete" class="delete"></td>
                </tr>
            """
        elif category == "All Categories" and len(query) == 0 or category == "All Categories" and query in listing['title'] or category in listing['category'] and len(query) == 0 or category in listing['category'] and query in listing['title']:
            renderedHTML = renderedHTML + f"""
                <tr data-image="{listing['image']}" data-description="{listing['title']}" class="tableRow" data-listing-id="{listing['id']}">
                    <td><a href={listing['link']} class="listingtitle">{listing['title']}</a></td>
                    <td>{listing['numbids']}</td>
                    <td>{escape_html(listing['category'])}</td>
                    <td>{typeset_dollars(listing['topbid'])}</td>
                    <td>{listing['date']}</td>
                    <td class="date">{listing['date']}</td>
                    <td><input type="button" value="Delete" class="delete"></td>
                </tr>
            """ 
    
    renderedHTML = renderedHTML + """
                        </tbody>
                    </table>
                    <div class="preview">
                        <div id="previewImg"></div>
                        <p id="previewDesc"></p>
                    </div>     
    """
    return renderedHTML + end_dyanmicHTML

def typeset_dollars(number):
    return f"${number:.2f}"


# The method signature is a bit "hairy", but don't stress it -- just check the documentation below.
# NOTE some people's computers don't like the type hints. If so replace below with simply: `def server(method, url, body, headers)`
# The type hints are fully optional in python.
def server(
    request_method: str,
    url: str,
    request_body: Optional[str],
    request_headers: dict[str, str],
) -> tuple[Union[str, bytes], int, dict[str, str]]:
    """
    `method` will be the HTTP method used, for our server that's GET, POST, DELETE, and maybe PUT
    `url` is the partial url, just like seen in previous assignments
    `body` will either be the python special `None` (if the body wouldn't be sent (such as in a GET request))
         or the body will be a string-parsed version of what data was sent.
    headers will be a python dictionary containing all sent headers.

    This function returns 3 things:
    The response body (a string containing text, or binary data)
    The response code (200 = ok, 404=not found, etc.)
    A _dictionary_ of headers. This should always contain Content-Type as seen in the example below.
    """
    response_body = None
    status = 200

    response_headers = {}

    parameters = None
    if "?" in url:
        url, parameters = url.split("?", 1)
      
    if parameters:
        if "&" in parameters and parameters[parameters.index('&') + 1] == '&' or parameters[parameters.index('=') + 1] == '=':
            response_body = open("static/html/error.html").read()
            status = 400
            response_headers["Content-Type"] = "text/html; charset=utf-8"
    
    if url.startswith("/api/"):
        rate_limit = pass_api_rate_limit()
        if rate_limit != 0:
            status = 429
            response_headers['Retry-After'] = rate_limit
        
    if request_method == "GET" and url == "/" or request_method == "GET" and url == "/main":
        response_body = open("static/html/mainpage.html").read()
        status = 200
        response_headers["Content-Type"] = "text/html; charset=utf-8"
    elif request_method == "GET" and url == "/gallery":
        if parameters:
            # Try to pass parameters into parse_query_parameters function. If error then generate 404
            try:
                parsedParams = parse_query_parameters(parameters)
            except:
                print("Gallery: First except reached")
                response_body = open("static/html/404.html").read()
                status = 404
                response_headers["Content-Type"] = "text/html; charset=utf-8"
            else:
                try:
                    # Try to pass parse_query_parameters result into render_gallery function. If error generate 404
                    render_gallary_result = render_gallery(parsedParams['query'], parsedParams['category'])
                except:
                    print("Gallery: Second except reached")
                    response_body = open("static/html/404.html").read()
                    status = 404
                    response_headers["Content-Type"] = "text/html; charset=utf-8"
                else:
                        # If nothing fails, write into gallery.html and set the variables
                        f = open("static/html/gallery.html", "w")
                        f.write(render_gallary_result)
                        f.close()
                        
                        response_body = open("static/html/gallery.html").read()
                        status = 200
                        response_headers["Content-Type"] = "text/html; charset=utf-8"
        else:
            # In the case where there aren't any parameters, render gallery with all categories
            render_gallary_result = render_gallery("", "All Categories")
            
            f = open("static/html/gallery.html", "w")
            f.write(render_gallary_result)
            f.close()
            
            response_body = open("static/html/gallery.html").read()
            status = 200
            response_headers["Content-Type"] = "text/html; charset=utf-8"

    elif request_method == "GET" and url.startswith("/listing"):        
        f = open("static/html/listing.html", "w")
        f.write(render_listing(url[-1], ""))
        f.close()
        
        response_body = open("static/html/listing.html").read()
        status = 200
        response_headers["Content-Type"] = "text/html; charset=utf-8"
    elif request_method == "GET" and url == "/create":
        response_body = open("static/html/create.html").read()
        status = 200
        response_headers["Content-Type"] = "text/html; charset=utf-8"
    elif request_method == "POST" and url == "/create":
        response_body = open("static/html/create.html").read()
        status = 200
        response_headers["Content-Type"] = "text/html; charset=utf-8"
    elif request_method == "POST" and url == "/api/place_bid": 
        # In the case where the body or header is empty. Or the header isn't json. Return 400 status code     
        if request_body is None or request_headers["Content-Type"] is None or "application/json" not in request_headers["Content-Type"]:
            response_body = ""
            status = 400
            response_headers["Content-Type"] = "application/json; charset=utf-8"
        # If header is json and body isn't none. call json.loads and find if there's any errors
        elif request_body is not None and "application/json" in request_headers["Content-Type"]:
            try:
                result = json.loads(request_body)
            except:
                print("POST place_bid: First Except Reached")
                response_body = ""
                status = 400
                response_headers["Content-Type"] = "application/json; charset=utf-8"
            else:
                dictCheck = isinstance(result, dict)
                nameCheck = isinstance(result['name'], str)
                amountCheck = isinstance(float(result['amount']), float) 
                idCheck = any(result['id'] in listing['id'] for listing in listings)
                
                # Any error with how the request_body was processed will result in 400 error
                if not dictCheck or not nameCheck or not amountCheck or not idCheck or result['name'] is None or not result['name'] or not result['amount'] or not result['comment']:
                    print("POST palce_bid: Second issue made")
                    response_body = ""
                    status = 400
                    response_headers["Content-Type"] = "application/json; charset=utf-8"
                # If the request_body's bid is less than the current top bid of the listing result in 409 error
                elif float(result['amount']) < listings[int(result['id']) - 1]['topbid']:
                    response_body = json.dumps(listings)
                    status = 409
                    response_headers["Content-Type"] = "application/json; charset=utf-8"
                # If request_body is valid, append request_body to listings and result in 201 code
                else:    
                    cookie = request_headers['Cookie']
                    if cookie['bidder_name'] is None:    
                        new_listing['name'] = result['name']
                    else:
                        new_listing['name'] = cookie['bidder_name']
                        
                    new_listing = {}               
                    new_listing['amount'] = float(result['amount'])              
                    new_listing['comment'] = result['comment']    
                    
                    listings[int(result['id']) - 1]['bids'].append(new_listing)
                    listings[int(result['id']) - 1]['topbid'] = float(new_listing['amount'])
                    listings[int(result['id']) - 1]['numbids'] = str(int(listings[int(result['id'])]['numbids']) + 1)
                
                    response_body = json.dumps(listings)
                    status = 201
                    response_headers["Content-Type"] = "application/json; charset=utf-8"
                    response_headers['Set-Cookie'] = result['name']
    elif request_method == "DELETE" and url == "/api/delete_listing":
        # For a malformed request_body or request_header, result 400 error
        if request_headers["Content-Type"] is None or "application/json" not in request_headers["Content-Type"] or request_body is None:
            response_body = ""
            status = 400
            response_headers["Content-Type"] = "application/json; charset=utf-8"
        # If request_header is json and the request_body isn't empty, load the body and check if there's an error
        elif request_body is not None and "application/json" in request_headers["Content-Type"]:
            try:
                result = json.loads(request_body)
            except:
                print("DELETE delete_listing: First Except Reached")
                response_body = ""
                status = 400
                response_headers["Content-Type"] = "application/json; charset=utf-8"
            else:
                print(result)
                idCheck = any(result['id'] in listing for listing in listings)
                    
                if not idCheck:
                    response_body = open("static/html/404.html").read()
                    status = 404
                    response_headers["Content-Type"] = "text/html; charset=utf-8"

                # If there wasn't any error from json load, check if the result is a dictionary and if "id" is in the result
                dictCheck = isinstance(result, dict)
                if not dictCheck or "id" not in result:
                    response_body = ""
                    status = 400
                    response_headers["Content-Type"] = "application/json; charset=utf-8"
                else:       
                    # Afterwards, iterate each listing and remove the intended one
                    for i in range(len(listings)):
                        if str(result['id']) == listings[i]['id']:
                            listings.pop(i - 1)
                            break

                    response_body = ""
                    status = 204
                    response_headers["Content-Type"] = "application/json; charset=utf-8"
                
    elif request_method == "GET" and url == "/images/main":
        response_body = open("static/images/VideoGames.jpg", "rb").read()
        status = 200
        response_headers["Content-Type"] = "image/jpeg; charset=utf-8"    
    elif request_method == "GET" and url == "/images/NES":
        response_body = open("static/images/NES.jpg", "rb").read()
        status = 200
        response_headers["Content-Type"] = "image/jpeg; charset=utf-8"    
    elif request_method == "GET" and url == "/images/PokemonYellow":
        response_body = open("static/images/PokemonYellow.jpg", "rb").read()
        status = 200
        response_headers["Content-Type"] = "image/jpeg; charset=utf-8"    
    elif request_method == "GET" and url == "/images/Pacman":
        response_body = open("static/images/Pacman.jpg", "rb").read()
        status = 200
        response_headers["Content-Type"] = "image/jpeg; charset=utf-8"    
    elif request_method == "GET" and url == "/main.css":
        response_body = open("static/css/main.css").read()
        status = 200
        response_headers["Content-Type"] = "text/css; charset=utf-8"
    elif request_method == "GET" and url == "/js/bid.js":
        response_body = open("static/js/bid.js").read()
        status = 200
        response_headers["Content-Type"] = "text/javascript; charset=utf-8"
    elif request_method == "GET" and url == "/js/new_listing.js":
        response_body = open("static/js/new_listing.js").read()
        status = 200
        response_headers["Content-Type"] = "text/javascript; charset=utf-8"
    elif request_method == "GET" and url == "/js/table.js":
        response_body = open("static/js/table.js").read()
        status = 200
        response_headers["Content-Type"] = "text/javascript; charset=utf-8"
    else:
        response_body = open("static/html/404.html").read()
        status = 404
        response_headers["Content-Type"] = "text/html; charset=utf-8"

    return response_body, status, response_headers


# You shouldn't need to change content below this. It would be best if you just left it alone.


class RequestHandler(BaseHTTPRequestHandler):
    
    def c_read_body(self):
        # Read the content-length header sent by the BROWSER
        content_length = int(self.headers.get("Content-Length", 0))
        # read the data being uploaded by the BROWSER
        body = self.rfile.read(content_length)
        # we're making some assumptions here -- but decode to a string.
        body = str(body, encoding="utf-8")
        return body

    def c_send_response(self, message, response_code, headers):
        # Convert the return value into a byte string for network transmission
        if type(message) == str:
            message = bytes(message, "utf8")

        # Send the first line of response.
        self.protocol_version = "HTTP/1.1"
        self.send_response(response_code)

        # Send headers (plus a few we'll handle for you)
        for key, value in headers.items():
            self.send_header(key, value)
        self.send_header("Content-Length", len(message))
        self.send_header("X-Content-Type-Options", "nosniff")
        self.end_headers()

        # Send the file.
        self.wfile.write(message)

    def do_POST(self):
        # Step 1: read the last bit of the request
        try:
            body = self.c_read_body()
        except Exception as error:
            # Can't read it -- that's the client's fault 400
            self.c_send_response(
                "Couldn't read body as text", 400, {"Content-Type": "text/plain"}
            )
            raise

        try:
            # Step 2: handle it.
            message, response_code, headers = server(
                "POST", self.path, body, self.headers
            )
            # Step 3: send the response
            self.c_send_response(message, response_code, headers)
        except Exception as error:
            # If your code crashes -- that's our fault 500
            self.c_send_response(
                "The server function crashed.", 500, {"Content-Type": "text/plain"}
            )
            raise

    def do_GET(self):
        try:
            # Step 1: handle it.
            message, response_code, headers = server(
                "GET", self.path, None, self.headers
            )
            # Step 3: send the response
            self.c_send_response(message, response_code, headers)
        except Exception as error:
            # If your code crashes -- that's our fault 500
            self.c_send_response(
                "The server function crashed.", 500, {"Content-Type": "text/plain"}
            )
            raise

    def do_DELETE(self):
        # Step 1: read the last bit of the request
        try:
            body = self.c_read_body()
        except Exception as error:
            # Can't read it -- that's the client's fault 400
            self.c_send_response(
                "Couldn't read body as text", 400, {"Content-Type": "text/plain"}
            )
            raise

        try:
            # Step 2: handle it.
            message, response_code, headers = server(
                "DELETE", self.path, body, self.headers
            )
            # Step 3: send the response
            self.c_send_response(message, response_code, headers)
        except Exception as error:
            # If your code crashes -- that's our fault 500
            self.c_send_response(
                "The server function crashed.", 500, {"Content-Type": "text/plain"}
            )
            raise


def run():
    PORT = 4131
    print(f"Starting server http://localhost:{PORT}/")
    server = ("", PORT)
    httpd = HTTPServer(server, RequestHandler)
    httpd.serve_forever()


run()

from http.server import BaseHTTPRequestHandler, HTTPServer

listings = [{'title' : 'NES', 'link' : "/listing/1", 'image' : '/images/NES', 'desc': "This is the Nintendo Entertainment System (NES). It is the highest of quality! The item has already sold off to the highest bidder."
            , 'numbids' : '12','category' : 'Consoles', 'id' : '1', 'topbid' : 22.21, 'date' : "3 days 1 hour 15 minutes", 'bids' : [{'name' : 'Jessie', 'amount' : 22.21, 'comment' : ''}, {'name' : 'James', 'amount' : 12.02, 'comment' : ''}, {'name' : 'Meowth', 'amount' : 17.72, 'comment' : "That's the name! I want this neowth!"}] },
            {'title' : 'Pokemon', 'link' : "/listing/2",'image' : '/images/PokemonYellow', 'desc': "This is the Pokemon Yellow. It is the highest of quality! The item has already sold off to the highest bidder."
            ,'numbids' : '1','category' : 'Video Games', 'id' : '2', 'topbid' : 2.00,'date' : "1 hour 2 minutes", 'bids' : [{'name' : 'Charizard', 'amount' : 22.21, 'comment' : ''}, {'name' : 'Pikachu', 'amount' : 12.02, 'comment' : "Pika Pika!"}, {'name' : 'Bulbasaur', 'amount' : 17.72, 'comment' : "" }]},
            {'title' : 'Pacman', 'link' : "/listing/3", 'image' : '/images/Pacman', 'desc': "This is a Pacman Cup. It is the highest of quality! The item has already sold off to the highest bidder."
            ,'numbids' : '3','category' : 'Merch', 'id' : '3', 'topbid' : 8.72, 'date' : "3 days 1 hour 15 minutes", 'bids' : [{'name' : 'Pacman', 'amount' : 22.21, 'comment' : ''}, {'name' : 'Cherry', 'amount' : 12.02, 'comment' : "Please don't eat me!"}, {'name' : 'Ghost', 'amount' : 17.72, 'comment' : "" }]}
            ]
# PUT YOUR GLOBAL VARIABLES AND HELPER FUNCTIONS HERE.

def append_gallery(elemParam):
    appended_data = f"""
                <tr>
                    <td><a href={elemParam['link']}>{elemParam['title']}</a></td>
                    <td>{elemParam['numbids']}</td>
                    <td>{elemParam['category']}</td>
                    <td>{typeset_dollars(elemParam['topbid'])}</td>
                    <td>{elemParam['date']}</td>
                </tr>
                """
    
    return appended_data


def escape_html(str):
    str = str.replace("&", "&amp;")
    str = str.replace('"', "&quot;")
    str = str.replace('>', "&gt;")
    str = str.replace('<', "&lt;")


    # you need more.

    return str


def unescape_url(url_str):
    import urllib.parse

    # NOTE -- this is the only place urllib is allowed on this assignment.
    return urllib.parse.unquote_plus(url_str)


def parse_query_parameters(response):
        if '&' in response[0]:
            # Split the query and parameter
            splitQ, *splitP = response[0].split("&")
            print(splitQ)
            print(splitP)
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
            print("Something went wrong")
            return {'query' : 'not', 'category' : 'found'}
def render_listing(listing):
    dynamicListing2 = """            
                    </div>
                </div>    
            </div>
        </body>
    </html>
    """

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
                </head>

                <body>
                    <nav>
                        <a href="/">Home</a>
                        <a href="/gallery">Listings</a>
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
                            <h3>Bids</h3>
                """
                
            dynamicListing2 = f"""
                            </div>
                        </div>    
                    </div>
                </body>
            </html>
            
            """
            
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
        </head>

        <body>
            <nav>
                <a href="/">Home</a>
                <a href="/gallery">Listings</a>
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
                    <table>
                        <thead>
                            <tr>
                                <th>Number of Lists</th>
                                <th>Number of Bids</th>
                                <th>Category</th>
                                <th>Top Bid</th>
                                <th>Time Left</th>        
                            </tr>
                        </thead>
                        <tbody>
        """
    
    dyanmicListings2 = """ 
                    </tbody>
                </table> 
            </body>
        </html>"""

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

# Provided function -- converts numbers like 42 or 7.347 to "$42.00" or "$7.35"
def typeset_dollars(number):
    return f"${number:.2f}"


def server(url):
    """
    url is a *PARTIAL* URL. If the browser requests `http://localhost:4131/contact?name=joe#test`
    then the `url` parameter will have the value "/contact?name=joe". So you can expect the PATH
    and any PARAMETERS from the url, but nothing else.

    This function is called each time another program/computer makes a request to this website.
    The URL represents the requested file.

    This function should return two strings in a list or tuple. The first is the content to return
    The second is the content-type.
    """
    # YOUR CODE GOES HERE!
    url, *parameters = url.split("?")

    if url == "/" or url == "/main":
        return (open("static/html/mainpage.html").read(), "text/html")
    elif url == "/gallery":
        if parameters:
            print(parameters[0])
            galleryParam = parse_query_parameters(parameters)

            if galleryParam['query'] == 'not' and galleryParam['category'] == 'found':
                return (open("static/html/404.html").read(), "text/html")
            else:
                render_gallary_result = render_gallery(galleryParam['query'], galleryParam['category'])

                if render_gallary_result == "/404":
                    return (open("static/html/404.html").read(), "text/html")
                else:    
                    f = open("static/html/dynamiclistings.html", "w")
                    f.write(render_gallary_result)
                    f.close()

                    return (open("static/html/dynamiclistings.html").read(), "text/html")
        else:
            return (open("static/html/listings.html").read(), "text/html")
    elif url.startswith("/listing"):

        f = open("static/html/dynamiclisting.html", "w")
        f.write(render_listing(url))
        f.close()

        return (open("static/html/dynamiclisting.html").read(), "text/html")
    elif url == "/images/VideoGames":
        return (open("static/images/VideoGames.jpg", "rb").read(), "images/jpeg")
    elif url == "/images/NES":
        return (open("static/images/NES.jpg", "rb").read(), "images/jpeg")
    elif url == "/images/PokemonYellow":
        return (open("static/images/PokemonYellow.jpg", "rb").read(), "images/jpeg")
    elif url == "/images/Pacman":
        return (open("static/images/Pacman.jpg", "rb").read(), "images/jpeg")
    elif url == "/main.css":
        return (open("static/css/main.css").read(), "text/css")
    else:
        return (open("static/html/404.html").read(), "text/html")   

# You shouldn't need to change content below this. It would be best if you just left it alone.


class RequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # Call the student-edited server code.
        message, content_type = server(self.path)

        # Convert the return value into a byte string for network transmission
        if type(message) == str:
            message = bytes(message, "utf8")

        # prepare the response object with minimal viable headers.
        self.protocol_version = "HTTP/1.1"
        # Send response code
        self.send_response(200)
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

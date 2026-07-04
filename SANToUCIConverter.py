from playwright.sync_api import sync_playwright

URL = "https://chess.video/tools/pgn-converter"

# Read in games line by line
file_path = "cmake-build-debug/games/games_uci.txt"
file = open(file_path, "r")
lines = file.readlines()

with sync_playwright() as p:
    browser = p.chromium.launch(headless=False)
    page = browser.new_page()

    page.goto(URL)

    
    page.get_by_text("SAN → UCI").click()



    content = ""
    counter = 0
    for line in lines:
        counter+=1
        page.get_by_role("textbox").fill(line)
        content += page.locator('.whitespace-pre-wrap').text_content()
        content += '\n'
        print(counter)
    browser.close()

# Hook into https://chess.video/tools/pgn-converter to convert games


# Write games back to new file
with open("cmake-build-debug/games/games_uci.txt", "w", encoding="utf-8") as f:
    f.write(content)



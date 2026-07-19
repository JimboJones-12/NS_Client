#include <WebPages.hpp>

namespace Web {
    const char AP_HTML_HOME[] = R"rawliteral(
        <!DOCTYPE html>
        <html>
            <head>
                <meta charset="UTF-8">
                <title>
                    Device Options
                </title>
                <link rel="stylesheet" href="styles.css">

                <script src="app.js" defer></script>
            </head>

            <body>
                <my-navbar></my-navbar>
                <my-header>Device Home</my-header>
                <main class="container">
                    <p>All changes will take effect after this site has been closed.</p><br>
                    <button type="reset">Reset all settings</button>
                </main>
            </body>
        </html>
    )rawliteral";

    const char AP_HTML_NS_OPTIONS[] = R"rawliteral(
        <!DOCTYPE html>
        <html>
            <head>
                <meta charset="UTF-8">
                <title>
                    Device Options
                </title>
                <link rel="stylesheet" href="styles.css">

                <script src="app.js" defer></script>
            </head>

            <body>
                <my-navbar></my-navbar>
                <my-header>NightScout Options</my-header>
                <main class="container">

                    <form>
                        <p>Data Type:</p>
                        <label>
                            <input type="radio" name="data-type" value="mmol/L">
                            mmol/L
                        </label><br>
                        <label>
                            <input type="radio" name="data-type" value="mg/dL">
                            mg/dL
                        </label><br>

                        <br>

                        <label>
                            NightScout Site URL:<br>
                            <input type="text" name="ns-site">
                        </label><br><br>
                        <label>
                            NightScout Secret:<br>
                            <input type="text" name="ns-secret">
                        </label><br>

                        <br>

                        <input type="submit" value="Apply Changes">
                    </form>
                </main>
            </body>
        </html>    
    )rawliteral";

    const char AP_HTML_DEVICE_OPTIONS[] = R"rawliteral(
        <!DOCTYPE html>
        <html>
            <head>
                <meta charset="UTF-8">
                <title>
                    Device Options
                </title>
                <link rel="stylesheet" href="styles.css">

                <script src="app.js" defer></script>
            </head>

            <body>
                <my-navbar></my-navbar>
                <my-header>WiFi Options</my-header>
                <main class="container">
                    <form>
                        <label>
                            WiFi network name:<br>
                            <input type="text" name="sta-ssid">
                        </label><br><br>
                        <label>
                            WiFi network password:<br>
                            <input type="text" name="sta-password">
                        </label><br>

                        <br><br>

                        <label>
                            Device network name:<br>
                            <input type="text" name="ap-ssid">
                        </label><br><br>
                        <label>
                            Device network password:<br>
                            <input type="text" name="ap-password">
                        </label><br>

                        <br>

                        <input type="submit" value="Apply Changes">
                    </form>
                </main>
            </body>
        </html>
        )rawliteral";

    const char AP_CSS[] = R"rawliteral(
        h1 {
        color: black;
        font-size: 2.5rem;
        font-family: sans-serif;
        text-align: center;
        }

        p {
        color: black;
        font-family: sans-serif;
        margin: 2px;
        }

        body {
        padding: 0px;
        margin: 0px;
        }

        .container {
        padding: 0px;
        margin: 8px;
        }

        form {
        color: black;
        font-family: sans-serif;
        margin: 2px;
        }

        form input {
        color: black;
        font-family: sans-serif;
        margin: 2px;
        }

        form label {
        color: black;
        font-family: sans-serif;
        margin: 2px;
        }
    )rawliteral";

    const char AP_JS[] = R"rawliteral(
        class MyNavbar extends HTMLElement {
        connectedCallback() {
            this.innerHTML = `
            <style>
                my-navbar {
                display: block;
                background-color: #333;
                }
                .navbar {
                display: flex;
                justify-content: left;
                }
                .navbar a {
                font-size: 0.9rem;
                display: block;
                color: white;
                text-align: center;
                padding: 12px 10px;
                text-decoration: none;
                font-family: sans-serif;
                }
                .navbar a:hover {
                background-color: #111111;
                }
                my-navbar a.active {
                color: #fff;
                border-bottom: 3px solid #00bcff;
                font-weight: normal;
                background-color: #282727;
                }
            </style>

            <nav class="navbar">
                <a href="home.html">Home</a>
                <a href="ns.html">NightScout Options</a>
                <a href="wifi.html">WiFi Options</a>
            </nav>
            `;

            const currentPath = window.location.pathname;
            const links = this.querySelectorAll('a');

            links.forEach(link => {
                const linkHref = link.getAttribute('href');
                if (currentPath.endsWith(linkHref) || (currentPath === '/' && linkHref === 'index.html')) {
                link.classList.add('active');
                }
            }
            );
        }
        }
        customElements.define('my-navbar', MyNavbar);

        class MyHeader extends HTMLElement {
        connectedCallback() {

            const innerText = this.textContent.trim();

            this.innerHTML = `
            <style>
                my-header div {
                color: white;
                font-size: 2rem;
                font-family: sans-serif;
                text-align: center;
                background-color: black;
                border-bottom: 2px solid #00bcff;
                border-top: 1px solid #00bcff;
                padding: 20px;
                }
            </style>

            <div>
            ${innerText}
            </div>
            `;
        }
        }
        customElements.define('my-header', MyHeader);
    )rawliteral";
}
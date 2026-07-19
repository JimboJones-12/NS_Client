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
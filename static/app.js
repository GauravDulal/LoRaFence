// navbar hamburger
const hamburger = document.getElementById("hamburger");
const dropdownMenu = document.getElementById("dropdown-menu");

hamburger.addEventListener("click", () => {
    const isHidden = dropdownMenu.classList.toggle("hidden");

    if (!isHidden) {
        dropdownMenu.style.maxHeight = `${dropdownMenu.scrollHeight}px`;
    } else {
        dropdownMenu.style.maxHeight = "0";
    }
    dropdownMenu.style.transitionDuration = "0.5s";
    hamburger.classList.toggle("hidden");
});

document.addEventListener("click", (event) => {
    if (
        !hamburger.contains(event.target) &&
        !dropdownMenu.contains(event.target)
    ) {
        dropdownMenu.classList.add("hidden");
        dropdownMenu.style.maxHeight = "0";
        hamburger.classList.remove("hidden");
    }
});










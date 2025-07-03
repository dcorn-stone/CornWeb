document.addEventListener('DOMContentLoaded', () => {
  const holes = document.querySelectorAll('.hole');
  const scoreEl = document.getElementById('score');
  const timerEl = document.getElementById('timer');
  const startBtn = document.getElementById('start');

  let lastHole;
  let timeUp = false;
  let score = 0;
  let gameTimer;
  let moleTimer;
  let timeLeft = 60;
  let popUpSpeed = 1000;

  function randomTime(min, max) {
    return Math.round(Math.random() * (max - min) + min);
  }

  function randomHole(holes) {
    const idx = Math.floor(Math.random() * holes.length);
    const hole = holes[idx];
    if (hole === lastHole) return randomHole(holes);
    lastHole = hole;
    return hole;
  }

  function peep() {
    const time = randomTime(popUpSpeed / 2, popUpSpeed);
    const hole = randomHole(holes);
    hole.classList.add('mole');
    setTimeout(() => {
      hole.classList.remove('mole');
      if (!timeUp) peep();
    }, time);
  }

  function startGame() {
    score = 0;
    timeLeft = 60;
    popUpSpeed = 1000;
    scoreEl.textContent = score;
    timerEl.textContent = timeLeft;
    timeUp = false;
    startBtn.disabled = true;

    peep();
    gameTimer = setInterval(() => {
      timeLeft--;
      timerEl.textContent = timeLeft;

      // speed up every 10 seconds
      if (timeLeft % 10 === 0 && popUpSpeed > 300) {
        popUpSpeed -= 100;
      }

      if (timeLeft <= 0) {
        timeUp = true;
        clearInterval(gameTimer);
        startBtn.disabled = false;
        alert(`Time's up! Your final score is ${score}.`);
      }
    }, 1000);
  }

  function whack(e) {
    if (!e.isTrusted) return; // ignore fake clicks
    if (!this.classList.contains('mole')) return;
    score++;
    scoreEl.textContent = score;
    this.classList.remove('mole');
    this.classList.add('hit');
    setTimeout(() => this.classList.remove('hit'), 200);
  }

  holes.forEach(hole => hole.addEventListener('click', whack));
  startBtn.addEventListener('click', startGame);
});


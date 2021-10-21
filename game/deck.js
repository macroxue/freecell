var suits = ['&spades;', '&hearts;', '&diams;', '&clubs;'];
var ranks = ['A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K'];
var selected_deck = 0;
var black = 0, red = 1;
var spades = 0, hearts = 1, diams = 2, clubs = 3;
var ace = 0, deuce = 1, king = 12;
var deck_pictures = [
  // clean
  ['', '', ''],
  // elf
  ['&#x1F9DD;', '&#x1F9DD;&#x200D;&#x2640;&#xFE0F;', '&#x1F9DD;&#x200D;&#x2642;&#xFE0F;'],
  // fairy
  ['&#x1F9DA;', '&#x1F9DA;&#x200D;&#x2640;&#xFE0F;', '&#x1F9DA;&#x200D;&#x2642;&#xFE0F;'],
  // genie
  ['&#x1F9DE;', '&#x1F9DE;&#x200D;&#x2640;&#xFE0F;', '&#x1F9DE;&#x200D;&#x2642;&#xFE0F;'],
  // hero
  ['&#x1F9B8;', '&#x1F9B8;&#x200D;&#x2640;&#xFE0F;', '&#x1F9B8;&#x200D;&#x2642;&#xFE0F;'],
  // mage
  ['&#x1F9D9;', '&#x1F9D9;&#x200D;&#x2640;&#xFE0F;', '&#x1F9D9;&#x200D;&#x2642;&#xFE0F;'],
  // vampire
  ['&#x1F9DB;', '&#x1F9DB;&#x200D;&#x2640;&#xFE0F;', '&#x1F9DB;&#x200D;&#x2642;&#xFE0F;'],
  // villain
  ['&#x1F9B9;', '&#x1F9B9;&#x200D;&#x2640;&#xFE0F;', '&#x1F9B9;&#x200D;&#x2642;&#xFE0F;'],
  // zombie
  ['&#x1F9DF;', '&#x1F9DF;&#x200D;&#x2640;&#xFE0F;', '&#x1F9DF;&#x200D;&#x2642;&#xFE0F;'],
];

// Helpers for card properties.
function suit(card) {
  return card & 3;
}
function rank(card) {
  return card >> 2;
}
function color(card) {
  return 1 <= suit(card) && suit(card) <= 2;
}
function get_card_id(card, deck = selected_deck) {
  return (suit(card) * 13 + rank(card)).toString() + '_' + deck.toString();
}
function get_card(card_id) {
  return (parseInt(card_id) % 13 << 2) + Math.floor(parseInt(card_id) / 13);
}

function create_clean_deck() {
  var rect = get_element_position('logo');
  for (var card = 0; card < 52; ++card) {
    var div = document.createElement('div');
    div.classList.add('card');
    div.id = get_card_id(card, 0);
    div.style.left = rect.left;
    div.style.top = rect.top;
    var card_color = color(card) == red ? 'redcard' : 'blackcard';
    var card_sign = ranks[rank(card)] + ' ' + suits[suit(card)];
    div.innerHTML = `
      <table class='${card_color}'>
        <tr>
          <td class='left'><div class='rank'>${card_sign}</div></td>
          <td class='right'><div class='rank'>${card_sign}</div></td>
        </tr>
      </table>`;
    document.body.appendChild(div);
  }
}

function create_picture_decks() {
  var rect = get_element_position('logo');
  for (var deck = 1; deck < deck_pictures.length; ++deck) {
    for (var card = 0; card < 52; ++card) {
      var div = document.createElement('div');
      div.classList.add('card');
      div.id = get_card_id(card, deck);
      div.style.left = rect.left;
      div.style.top = rect.top;
      var card_color = color(card) == red ? 'redcard' : 'blackcard';
      var suit_sign = suits[suit(card)];
      div.innerHTML = `
      <table class='${card_color}'>
        <tr>${card == 0 ? standard_spade_ace : standard_deck[rank(card)]}</tr>
      </table>`.replace(/{suit_sign}/g, suit_sign)
        .replace(/{jack}/g, deck_pictures[deck][0])
        .replace(/{queen}/g, deck_pictures[deck][1])
        .replace(/{king}/g, deck_pictures[deck][2]);
      document.body.appendChild(div);
    }
  }
}

function select_deck() {
  var old_deck = [];
  for (var i = 0; i < 52; ++i) {
    old_deck.push(get_card_id(i));
  }
  selected_deck = document.getElementById('select_deck').value;
  for (var i = 0; i < 52; ++i) {
    var old_card = document.getElementById(old_deck[i]);
    var new_card = document.getElementById(get_card_id(i));
    old_card.style.display = 'none';
    new_card.style.display = 'block';
    new_card.style.left = old_card.style.left;
    new_card.style.top = old_card.style.top;
    new_card.style.zIndex = old_card.style.zIndex;
    new_card.onclick = old_card.onclick;
    new_card.ondragover = old_card.ondragover;
    new_card.setAttribute('draggable', old_card.getAttribute('draggable'));
  }
  set_cookie('deck', selected_deck.toString());
  return;
}

var standard_spade_ace = `
    <td class='left'><div class='rank'>A</div><div class='suit'>{suit_sign}</div></td>
    <td class='center_large'><div class='up'>{suit_sign}</div></td>
    <td class='right'><div class='rank'>A</div><div class='suit'>{suit_sign}</div></td>
  `;

var standard_deck = [
  `
    <td class='left'><div class='rank'>A</div><div class='suit'>{suit_sign}</div></td>
    <td class='center'><div class='up'>&nbsp;{suit_sign}&nbsp;</div></td>
    <td class='right'><div class='rank'>A</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>2</div><div class='suit'>{suit_sign}</div></td>
    <td class='center'>&nbsp;</td>
    <td class='center'>
      <div class='up'>&nbsp;{suit_sign}&nbsp;<br/>&nbsp;</div>
      <div class='down'>{suit_sign}<br/>&nbsp;</div>
    </td>
    <td class='center'>&nbsp;</td>
    <td class='right'><div class='rank'>2</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>3</div><div class='suit'>{suit_sign}</div></td>
    <td class='center'>&nbsp;</td>
    <td class='center'>
      <div class='up'>&nbsp;{suit_sign}&nbsp;</div>
      <div class='pad'>&nbsp;</div>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='down'>{suit_sign}</div>
    </td>
    <td class='center'>&nbsp;</td>
    <td class='right'><div class='rank'>3</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>4</div><div class='suit'>{suit_sign}</div></td>
    <td class='center'>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='up'>&nbsp;</div>
      <div class='pad'>&nbsp;</div>
      <div class='down'>{suit_sign}</div>
    </td>
    <td class='center'>&nbsp;</td>
    <td class='center'>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='up'>&nbsp;</div>
      <div class='pad'>&nbsp;</div>
      <div class='down'>{suit_sign}</div>
    </td>
    <td class='right'><div class='rank'>4</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>5</div><div class='suit'>{suit_sign}</div></td>
    <td class='center'>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='up'>&nbsp;</div>
      <div class='pad'>&nbsp;</div>
      <div class='down'>{suit_sign}</div>
    </td>
    <td class='center'><div class='up'>{suit_sign}</div></td>
    <td class='center'>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='up'>&nbsp;</div>
      <div class='pad'>&nbsp;</div>
      <div class='down'>{suit_sign}</div>
    </td>
    <td class='right'><div class='rank'>5</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>6</div><div class='suit'>{suit_sign}</div></td>
    <td class='center'>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='down'>{suit_sign}</div>
    </td>
    <td class='center'>&nbsp;</td>
    <td class='center'>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='down'>{suit_sign}</div>
    </td>
    <td class='right'><div class='rank'>6</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>7</div><div class='suit'>{suit_sign}</div></td>
    <td class='center'>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='down'>{suit_sign}</div>
    </td>
    <td class='center'>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;<br/>&nbsp;<br/>&nbsp;</div>
    </td>
    <td class='center'>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='down'>{suit_sign}</div>
    </td>
    <td class='right'><div class='rank'>7</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>8</div><div class='suit'>{suit_sign}</div></td>
    <td class='center'>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='down'>{suit_sign}</div>
    </td>
    <td class='center'>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='down'>{suit_sign}</div>
    </td>
    <td class='center'>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='up'>{suit_sign}</div>
      <div class='pad'>&nbsp;</div>
      <div class='down'>{suit_sign}</div>
    </td>
    <td class='right'><div class='rank'>8</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>9</div><div class='suit'>{suit_sign}</div></td>
    <td class='center'>
      <div class='up'>{suit_sign}<br/>{suit_sign}</div>
      <div class='down'>{suit_sign}<br/>{suit_sign}</div>
    </td>
    <td class='center'><div class='up'>{suit_sign}</div></td>
    <td class='center'>
      <div class='up'>{suit_sign}<br/>{suit_sign}</div>
      <div class='down'>{suit_sign}<br/>{suit_sign}</div>
    </td>
    <td class='right'><div class='rank'>9</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank2'>10</div><div class='suit'>{suit_sign}</div></td>
    <td class='center'>
      <div class='up'>{suit_sign}<br/>{suit_sign}</div>
      <div class='down'>{suit_sign}<br/>{suit_sign}</div>
    </td>
    <td class='center'>
      <div class='up'>{suit_sign}<br/>&nbsp;</div>
      <div class='down'>{suit_sign}</div>
    </td>
    <td class='center'>
      <div class='up'>{suit_sign}<br/>{suit_sign}</div>
      <div class='down'>{suit_sign}<br/>{suit_sign}</div>
    </td>
    <td class='right'><div class='rank2'>10</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>J</div><div class='suit'>{suit_sign}</div></td>
    <td class='picture'>{jack}</td>
    <td class='right'><div class='rank'>J</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>Q</div><div class='suit'>{suit_sign}</div></td>
    <td class='picture'>{queen}</td>
    <td class='right'><div class='rank'>Q</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>K</div><div class='suit'>{suit_sign}</div></td>
    <td class='picture'>{king}</td>
    <td class='right'><div class='rank'>K</div><div class='suit'>{suit_sign}</div></td>
  `
];

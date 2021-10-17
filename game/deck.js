var suits = ['&spades;', '&hearts;', '&diams;', '&clubs;'];
var ranks = ['A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K'];
var selected_deck = 0;

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
    var card_color = color(card) ? 'redcard' : 'blackcard';
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

function create_standard_deck() {
  var rect = get_element_position('logo');
  for (var card = 0; card < 52; ++card) {
    var div = document.createElement('div');
    div.classList.add('card');
    div.id = get_card_id(card, 1);
    div.style.left = rect.left;
    div.style.top = rect.top;
    var card_color = color(card) ? 'redcard' : 'blackcard';
    var suit_sign = suits[suit(card)];
    div.innerHTML = `
      <table class='${card_color}'>
        <tr>${card == 0 ? standard_spade_ace : standard_deck[rank(card)]}</tr>
      </table>`.replace(/{suit_sign}/g, suit_sign);
    document.body.appendChild(div);
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
  }
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
    <td class='center'><div class='up'>{suit_sign}</div></td>
    <td class='right'><div class='rank'>A</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>2</div><div class='suit'>{suit_sign}</div></td>
    <td class='center'>&nbsp;</td>
    <td class='center'>
      <div class='up'>{suit_sign}<br/>&nbsp;</div>
      <div class='down'>{suit_sign}<br/>&nbsp;</div>
    </td>
    <td class='center'>&nbsp;</td>
    <td class='right'><div class='rank'>2</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>3</div><div class='suit'>{suit_sign}</div></td>
    <td class='center'>&nbsp;</td>
    <td class='center'>
      <div class='up'>{suit_sign}</div>
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
    <td class='center'>
      <div class='up'>{suit_sign}&nbsp;<font style='font-size:200%'>J</font></div>
      <div class='down'>{suit_sign}&nbsp;<font style='font-size:200%'>J</font></div>
    </td>
    <td class='right'><div class='rank'>J</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>Q</div><div class='suit'>{suit_sign}</div></td>
    <td class='center'>
      <div class='up'>{suit_sign}<font style='font-size:200%'>Q</font></div>
      <div class='down'>{suit_sign}<font style='font-size:200%'>Q</font></div>
    </td>
    <td class='right'><div class='rank'>Q</div><div class='suit'>{suit_sign}</div></td>
  `,
  `
    <td class='left'><div class='rank'>K</div><div class='suit'>{suit_sign}</div></td>
    <td class='center'>
      <div class='up'>{suit_sign}<font style='font-size:200%'>K</font></div>
      <div class='down'>{suit_sign}<font style='font-size:200%'>K</font></div>
    </td>
    <td class='right'><div class='rank'>K</div><div class='suit'>{suit_sign}</div></td>
  `
];

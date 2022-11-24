#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define HEARTS   1
#define CLUBS    2
#define DIAMONDS 3
#define SPADES   4

struct card 
{
    int soft_val;
    int hard_val;
    int suit;
    char* name;
};

char* get_card_name(int value)
{
    switch(value)
    {
        case 1:
            return "Ace";
        case 2:
            return "Two";
        case 3:
            return "Three";
        case 4:
            return "Four";
        case 5:
            return "Five";
        case 6:
            return "Six";
        case 7:
            return "Seven";
        case 8:
            return "Eight";
        case 9:
            return "Nine";
        case 10:
            return "Ten";
        case 11:
            return "Jack";
        case 12:
            return "Queen";
        case 13:
            return "King";
        default:
            return "Error";
    }
}

char* get_suit_name(int i)
{
    switch(i) 
    {
        case 1: return "HEARTS";
        case 2: return "CLUBS";
        case 3: return "DIAMONDS";
        case 4: return "SPADES";
        default: return "ERROR";
    }
}

int get_suit(int i)
{
    if (i < 13)
        return HEARTS;
    if (i < 26)
        return CLUBS;
    if (i < 39)
        return DIAMONDS;
    return SPADES;
}

void trace(int i)
{
    printf("[---] Error here - %d\n", i);
}

char* card_to_str(struct card* card)
{
    char *ret = malloc(65);
    ret[0] = '\0';
    snprintf(ret, 64, " suit=%s :: soft_val=%d :: hard_val=%d :: name=%s\n",
             get_suit_name(card->suit), card->soft_val, card->hard_val, card->name);
    return ret;
}

int init_deck(struct card** deck)
{
    struct card *_deck = malloc(52 * sizeof(struct card));
    struct card *top;
    for (int i = 0; i < 52; i++)
    {
        int val = (i % 13) + 1;
        _deck[i].suit = get_suit(i);
        _deck[i].name = get_card_name(val);
        int soft_val = val > 10 ? 10 : val;
        _deck[i].soft_val = soft_val;
        _deck[i].hard_val = soft_val == 1 ? 11 : soft_val;
    }
    *deck = _deck;
    return 0;
}

int shuffle_deck(struct card** deck)
{
    srand(time(NULL));
    int buckets[52];
    for (int i = 0; i < 52; i++)
        buckets[i] = 1;
    int cond = 52, seq = 0;
    struct card* d = *deck;
    struct card* shuffled_deck = malloc(52 * sizeof(struct card));
    int idx;
    while (cond)
    {
        idx = rand() % 52;
        if (buckets[idx])
        {
            buckets[idx] = 0;
            cond--;
            shuffled_deck[seq++] = d[idx];
        }
    }
    *deck = shuffled_deck;
    free(d);
    return 0;
}

int sanitize_input(char c)
{
    switch(c)
    {
        case 'h':
        case 's':
            return 0;
        default:
            return 1;
    }
}

int evaluate_hand(struct card *player, int hand_size)
{
    int aces_exist = 0, hard_sum = 0;
    for (int i = 0; i < hand_size; i++)
    {
        if (player[i].soft_val == 1)
            aces_exist++;
        hard_sum += player[i].hard_val;
    }

    if (hand_size == 2 && hard_sum == 21)
        return -1;

    if (aces_exist)
    {
        int combinations = aces_exist + 1;
        int sums[combinations];
        int non_ace_sum = hard_sum - (11 * aces_exist);
        sums[0] = non_ace_sum + aces_exist;
        for (int i = 1; i <= aces_exist; i++)
            sums[i] = non_ace_sum + aces_exist + (i * 10);
        int max_result = 0;
        for (int i = 0; i < combinations; i++)
            if (sums[i] <= 21)
                max_result = max_result >= sums[i] ? max_result: sums[i];
        // busted
        if (!max_result)
            return -2;
        return max_result;
    }

    if (hard_sum > 21)
        return -2;
    return hard_sum;
}

char get_user_action()
{
    char action = getc(stdin);
    int err = sanitize_input(action);
    while (err)
    {
        printf("[---] 'h' to HIT or 's' To Stay\n");
        action = getc(stdin);
        err = sanitize_input(action);
    }
    return action;
}

int deal(struct card **player_hand,
              struct card **house_hand,
              struct card *deck)
{
    int p_i = 0, h_i = 0, d_i = 0;
    struct card *player_first_card = *player_hand + p_i++;
    struct card *player_second_card = *player_hand + p_i++;
    struct card *house_first_card = *house_hand + h_i++;
    struct card *house_second_card = *house_hand + h_i++;
    *player_first_card = deck[d_i++];
    *house_first_card = deck[d_i++];
    *player_second_card = deck[d_i++];
    *house_second_card = deck[d_i++];
    // printf("[---] Player hand\nCard1=%sCard2=%s",
    //        card_to_str(*player_hand), card_to_str(*player_hand + 1));
    // printf("[---] House Card\nCard%s", card_to_str(*house_hand + 1));
    return 0;
}

int begin_round(struct card *deck)
{
    int p_i = 2, h_i = 2, d_i = 4, err = 0;
    struct card *player_hand = malloc(10 * sizeof(struct card));
    struct card *house_hand = malloc(10 * sizeof(struct card));
    err = deal(&player_hand, &house_hand, deck);
    int player_hand_value = 0, house_hand_value = 0;
    printf("[---] Player Hand\nCard1=%sCard2=%s",
           card_to_str(player_hand), card_to_str(player_hand + 1));
    printf("[---] House Hand\nCard=%s", card_to_str(house_hand + 1));
    player_hand_value = evaluate_hand(player_hand, p_i);
    house_hand_value = evaluate_hand(house_hand, h_i);
    printf("[---] Player Currnet Hand Value %d\n", player_hand_value);
    // player blackjack
    if (player_hand_value == -1 && house_hand_value != -1)
        return 2;
    // push
    if (player_hand_value == -1 && house_hand_value == -1)
        return 0;
    if (house_hand_value == -1)
    {
        printf("[---] House Black Jack!\n");
        return -1;
    }
    printf("[---] Would you like to Hit(h) Or Stay(s)?\n");
    char action = get_user_action();
    while (action != 's')
    {
        player_hand[p_i++] = deck[d_i++];
        player_hand_value = evaluate_hand(player_hand, p_i);
        printf("[---] You Got %s\nYour Current Hand Value is %d\n",
               card_to_str(&player_hand[p_i - 1]), player_hand_value);
        // bust
        if (player_hand_value == -2)
            return -1;
        action = get_user_action();
    }
    printf("House Hand\nCard1=%sCard2=%sHouse Hand Value is %d\n",
           card_to_str(house_hand), card_to_str(house_hand + 1),
           house_hand_value);
    while(house_hand_value < 17 && house_hand_value > 0)
    {
        house_hand[h_i++] = deck[d_i++];
        printf("[---] House Got %s", card_to_str(&house_hand[h_i - 1]));
        house_hand_value = evaluate_hand(house_hand, h_i);
        // bust
        if (house_hand_value == -2)
        {
            printf("[---] House Busted!\n");
            return 1;
        }
        printf("[---] Current House Hand Value :: %d\n", house_hand_value);
    }

    if (house_hand_value == player_hand_value)
        return 0;
    else if (house_hand_value < player_hand_value)
        return 1;
    else
        return -1;
}

int begin_round_old(struct card* deck)
{
    // deal
    int *player_idx, *house_idx, *deck_idx, err = 0;
    struct card *player_hand = malloc(10 * sizeof(struct card));
    struct card *house_hand = malloc(10 * sizeof(struct card));

    err = deal(&player_hand, &player_hand, deck);
    int player_hand_val = evaluate_hand(player_hand, 2);
    printf("[---] Player hand Val is %d\n", player_hand_val);
    if (player_hand_val == -1)
    {
        if ((house_hand->hard_val + (house_hand + 1)->hard_val) == 21)
        {
            printf("[---] Both player and house got black jacks\n");
            return 0;
        }
        return 1;
    }

    printf("[!!!] player_idx=%d house_idx=%d deck_idx=%d\n", *player_idx, *house_idx, *deck_idx);
    printf("[---] Would you like to Hit(h) Or Stand(s)?\n");
    // this is shit?
    char action = get_user_action();
    while (action != 's')
    {
        struct card *player_new_card = player_hand + *player_idx++;
        // player_hand[*player_idx++] = temp;
        player_new_card = deck + *deck_idx++;
        printf("[---] Player Drew Card=%s", card_to_str(player_new_card));
        player_hand_val = evaluate_hand(player_hand, *player_idx);
        // bust
        if (player_hand_val == -2)
            return player_hand_val;
        action = get_user_action();
    }
    // if user stays then deal for house till the end;
    int house_hand_val = evaluate_hand(house_hand, *house_idx);
    printf("[---] House Current Value = %d\n", house_hand_val);
    // house stops at hard 17
    while(house_hand_val < 17)
    {
        house_hand[*house_idx++] = deck[*deck_idx++];
        house_hand_val = evaluate_hand(house_hand, *house_idx);
        if (house_hand_val == -2)
        {
            printf("[---] House Busted!\n");
            break;
        }
        printf("[---] House Current Value = %d\n", house_hand_val);
    }
    // player won
    if (house_hand_val > 21 || house_hand_val < player_hand_val)
        return 1;
    // push
    if (house_hand_val == player_hand_val)
        return 0;
    // House won
    return -1;
}

int test_evaluate_hand_blackjack()
{
    struct card player_hand[2];
    player_hand[0].hard_val = 11;
    player_hand[0].soft_val = 1;
    player_hand[0].suit = 4;
    player_hand[1].hard_val = 10;
    player_hand[1].soft_val = 10;
    player_hand[1].suit = 4;
    return evaluate_hand(player_hand, 2);
}

int test_evaluate_hand_bust()
{
    struct card player_hand[3];
    player_hand[0].hard_val = 10;
    player_hand[0].soft_val = 10;
    player_hand[0].suit = 4;
    player_hand[1].hard_val = 10;
    player_hand[1].soft_val = 10;
    player_hand[1].suit = 4;
    player_hand[2].hard_val = 10;
    player_hand[2].soft_val = 10;
    player_hand[2].suit = 4;
    return evaluate_hand(player_hand, 3);
}

int test_evaluate_hand_non_bust_no_ace()
{
    struct card player_hand[2];
    player_hand[0].hard_val = 10;
    player_hand[0].soft_val = 10;
    player_hand[0].suit = 4;
    player_hand[1].hard_val = 10;
    player_hand[1].soft_val = 10;
    player_hand[1].suit = 4;
    return evaluate_hand(player_hand, 2);
}

int test_evaluate_hand_non_bust_ace()
{
    struct card player_hand[2];
    player_hand[0].hard_val = 11;
    player_hand[0].soft_val = 1;
    player_hand[0].suit = 4;
    player_hand[1].hard_val = 5;
    player_hand[1].soft_val = 5;
    player_hand[1].suit = 4;
    return evaluate_hand(player_hand, 2);
}

int test_evaluate_hand_non_bust_2aces()
{
    struct card player_hand[3];
    player_hand[0].hard_val = 11;
    player_hand[0].soft_val = 1;
    player_hand[0].suit = 4;
    player_hand[1].hard_val = 5;
    player_hand[1].soft_val = 5;
    player_hand[1].suit = 4;
    player_hand[2].hard_val = 11;
    player_hand[2].soft_val = 1;
    player_hand[2].suit = 3;
    return evaluate_hand(player_hand, 3);
}

int test_evaluate_hand_draw_ace()
{
    struct card *player_hand = malloc(3 * sizeof(struct card));
    player_hand[0].hard_val = 11;
    player_hand[0].soft_val = 1;
    player_hand[0].suit = 3;
    player_hand[0].name = "Ace";
    player_hand[1].hard_val = 5;
    player_hand[1].soft_val = 5;
    player_hand[1].name = "Five";
    player_hand[1].suit = 2;
    int player_hand_value = evaluate_hand(player_hand, 2);
    printf("[---] Player hand value %d\n", player_hand_value);
    player_hand[2].hard_val = 10;
    player_hand[2].soft_val = 10;
    player_hand[2].name = "Queen";
    player_hand[2].suit = 1;
    player_hand_value = evaluate_hand(player_hand, 3);
    printf("[---] Player hand value %d\n", player_hand_value);
    return 0;
}

int test_evaluate_hand()
{
    int err1 = test_evaluate_hand_blackjack();
    if (err1 != -1)
        printf("[---] Black Jack Test Failed\n");
    else
        printf("[---] Black Jack Test Passed\n");

    err1 = test_evaluate_hand_bust();
    if (err1 == -2)
        printf("[---] Bust Test Passed!\n");
    else
        printf("[---] Bust Test Test Failed, result=%d\n", err1);
    
    err1 = test_evaluate_hand_non_bust_no_ace();
    if (err1 == 20)
        printf("[---] Non Bust No Ace Test Passed\n");
    else
        printf("[---] Non Bust No Ace Test Failed\n");

    err1 = test_evaluate_hand_non_bust_ace();
    if (err1 == 16)
        printf("[---] Non Bust Ace Test Passed\n");
    else
        printf("[---] Non Bust Ace Test Failed\n");

    err1 = test_evaluate_hand_non_bust_2aces();
    if (err1 == 17)
        printf("[---] Non Bust 2 Aces Test Passed\n");
    else
        printf("[---] Non Bust 2 Aces Test Failed\n");

    return 0;
}

int test_card_to_str()
{
    struct card *player_card = malloc(sizeof(struct card));
    player_card->hard_val = 7;
    player_card->soft_val = 7;
    player_card->suit = 4;
    player_card->name = get_card_name(7);
    printf("[---] Card is\n%s\n", card_to_str(player_card));
    return 0;
}

int test_deal()
{
    struct card *player_hand = malloc(sizeof(struct card) * 10);
    struct card *house_hand = malloc(sizeof(struct card) * 10);
    struct card *deck1 = malloc(sizeof(struct card) * 52);
    int *player_idx, *house_idx, *deck_idx;
    init_deck(&deck1);
    deal(&player_hand, &house_hand, deck1);
    printf("[---] Player Hand\nCard1=%sCard2=%s", card_to_str(player_hand), card_to_str(player_hand + 1));
    printf("[---] House Hand\nCard1=%sCard2=%s", card_to_str(house_hand), card_to_str(house_hand + 1));
    return 0;
}

int test_begin_round()
{
    struct card *deck = malloc(sizeof(struct card) * 52);
    init_deck(&deck);
    begin_round(deck);
    return 0;
}


int main(int argc, char **argv)
{
    // test
    // test_card_to_str(); // passed
    // test_evaluate_hand();
    // test_deal();
    // test_begin_round();
    // test_evaluate_hand_draw_ace();
    // return 0;

    // actual game
    struct card* deck = malloc(sizeof(struct card) * 52);

    int err = init_deck(&deck);
    if (err)
        printf("[+++] Error during initializing deck\n");

    err = shuffle_deck(&deck);
    if (err)
        printf("[+++] Error during shuffling deck\n");

    err = begin_round(deck);
    if (err > 0)
        printf("[---] Player Won!!! Congratulations!\n");
    if (!err)
        printf("[---] Push!\n");
    if (err < 0)
        printf("[---] Tough luck, house won\n");

    // Todo -- add tracking of player budget and calculate wins / loses

    return 0;
}


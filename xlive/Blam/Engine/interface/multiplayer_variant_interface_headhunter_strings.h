#pragma once

#include "main/game_preferences.h"
#include "saved_games/game_variant.h"

const static wchar_t* g_multiplayer_variant_interface_headhunter_parameter_title_strings[k_language_count][k_multiplayer_variant_headhunter_parameter_count] =
{
    // English
    {
        L"Moving Bin",
        L"Point Multiplier",
        L"Suicide Point Loss",
        L"Death Point Loss",
        L"Uncontested Bin",
        L"Speed With Heads",
        L"Max Heads Carried"
    },
    // Japanese
    {
        L"移動ビン",
        L"得点倍率",
        L"自殺による減点",
        L"死亡による減点",
        L"競争なしビン",
        L"頭部保持時の速度",
        L"最大保持頭数"
    },
    // German
    {
        L"Bewegliche Tonne",
        L"Punkte-Multiplikator",
        L"Punkteverlust bei Selbstmord",
        L"Punkteverlust bei Tod",
        L"Unangefochtene Tonne",
        L"Geschwindigkeit beim Tragen von Köpfen",
        L"Maximal getragene Köpfe"
    },
    // French
    {
        L"Bac mobile",
        L"Multiplicateur de points",
        L"Perte de points en cas de suicide",
        L"Perte de points en cas de mort",
        L"Bac non contesté",
        L"Vitesse en transportant des têtes",
        L"Nombre maximal de têtes transportées"
    },
    // Spanish
    {
        L"Contenedor móvil",
        L"Multiplicador de puntos",
        L"Pérdida de puntos por suicidio",
        L"Pérdida de puntos por muerte",
        L"Contenedor sin competencia",
        L"Velocidad al transportar cabezas",
        L"Máximo de cabezas transportadas"
    },
    // Italian
    {
        L"Contenitore mobile",
        L"Moltiplicatore di punti",
        L"Perdita di punti per suicidio",
        L"Perdita di punti per morte",
        L"Contenitore non contestato",
        L"Velocità durante il trasporto delle teste",
        L"Numero massimo di teste trasportate"
    },
    // Korean
    {
        L"이동식 통",
        L"점수 배수",
        L"자살 시 점수 감소",
        L"사망 시 점수 감소",
        L"경쟁 없는 통",
        L"머리 보유 시 속도",
        L"최대 보유 머리 수"
    },
    // Chinese
    {
        L"移动箱",
        L"得分乘数",
        L"自杀扣分",
        L"死亡扣分",
        L"无竞争箱",
        L"携带头颅时的速度",
        L"最大携带头颅数"
    },
    // Portuguese
    {
        L"Caixa móvel",
        L"Multiplicador de pontos",
        L"Perda de pontos por suicídio",
        L"Perda de pontos por morte",
        L"Caixa sem competição",
        L"Velocidade ao transportar cabeças",
        L"Máximo de cabeças transportadas"
    }
};

const static wchar_t* g_multiplayer_variant_interface_headhunter_parameter_description_strings[k_language_count][k_multiplayer_variant_headhunter_parameter_count] =
{
    // English
    {
        L"Controls if the head deposit bin moves locations.",
        L"Tokens can be 1 point each, or multiplied by the amount you deposit.",
        L"This option takes a point away from your score each time you suicide.",
        L"This option takes a point away from your score each time you die.",
        L"Determines if players cannot score if there is another player occupying the bin.",
        L"This setting makes head carriers run faster or slower than other players.",
        L"Sets how many heads a player can carry at a time"
    },
    // Japanese
    {
        L"頭部を預けるビンが位置を移動するかどうかを制御します。",
        L"トークンは1つにつき1ポイント、または預ける量に応じて倍率が適用されます。",
        L"自殺するたびにスコアから1ポイント減算されます。",
        L"死亡するたびにスコアから1ポイント減算されます。",
        L"他のプレイヤーがビンを占有している場合、プレイヤーが得点できないかどうかを決定します。",
        L"頭部を運ぶプレイヤーの走行速度を他のプレイヤーより速くまたは遅くします。",
        L"一度にプレイヤーが運べる頭数を設定します。"
    },
    // German
    {
        L"Bestimmt, ob die Kopf-Einzahlungsbox ihre Position ändert.",
        L"Token sind entweder 1 Punkt pro Stück oder werden mit der eingezahlten Menge multipliziert.",
        L"Diese Option zieht bei jedem Selbstmord einen Punkt von deinem Score ab.",
        L"Diese Option zieht bei jedem Tod einen Punkt von deinem Score ab.",
        L"Bestimmt, ob Spieler keine Punkte erhalten, wenn ein anderer Spieler die Box besetzt.",
        L"Diese Einstellung lässt Kopfträger schneller oder langsamer laufen als andere Spieler.",
        L"Legt fest, wie viele Köpfe ein Spieler gleichzeitig tragen kann."
    },
    // French
    {
        L"Contrôle si la benne de dépôt des têtes change de position.",
        L"Les jetons valent 1 point chacun ou sont multipliés en fonction de la quantité déposée.",
        L"Cette option retire un point de votre score à chaque suicide.",
        L"Cette option retire un point de votre score à chaque mort.",
        L"Détermine si les joueurs ne peuvent pas marquer lorsque la benne est occupée par un autre joueur.",
        L"Ce réglage fait courir les porteurs de tête plus vite ou plus lentement que les autres joueurs.",
        L"Définit le nombre de têtes qu'un joueur peut porter simultanément."
    },
    // Spanish
    {
        L"Controla si el contenedor de cabezas se desplaza a otras ubicaciones.",
        L"Los tokens pueden valer 1 punto cada uno o multiplicarse según la cantidad que deposites.",
        L"Esta opción resta un punto a tu puntuación cada vez que te suicidas.",
        L"Esta opción resta un punto a tu puntuación cada vez que mueres.",
        L"Determina si los jugadores no pueden anotar si otro jugador está ocupando el contenedor.",
        L"Esta configuración hace que los portadores de cabezas corran más rápido o más lento que los demás jugadores.",
        L"Establece cuántas cabezas puede llevar un jugador a la vez."
    },
    // Italian
    {
        L"Controlla se il bidone per la consegna delle teste cambia posizione.",
        L"I gettoni possono valere 1 punto ciascuno o essere moltiplicati in base alla quantità depositata.",
        L"Questa opzione sottrae un punto al tuo punteggio ogni volta che ti suicidi.",
        L"Questa opzione sottrae un punto al tuo punteggio ogni volta che muori.",
        L"Determina se i giocatori non possono segnare se un altro giocatore occupa il bidone.",
        L"Questa impostazione fa correre i portatori di teste più velocemente o più lentamente rispetto agli altri giocatori.",
        L"Imposta quante teste un giocatore può trasportare contemporaneamente."
    },
    // Korean
    {
        L"머리 보관함이 위치를 이동하는지 여부를 제어합니다.",
        L"토큰은 각각 1점이거나, 예치한 양에 따라 배수로 계산됩니다.",
        L"자살할 때마다 점수가 1점씩 차감됩니다.",
        L"죽을 때마다 점수가 1점씩 차감됩니다.",
        L"다른 플레이어가 보관함을 차지하고 있을 경우, 플레이어가 점수를 얻을 수 없는지를 결정합니다.",
        L"이 설정은 머리를 운반하는 플레이어가 다른 플레이어보다 빠르거나 느리게 달리도록 합니다.",
        L"한 번에 플레이어가 운반할 수 있는 머리의 수를 설정합니다."
    },
    // Chinese
    {
        L"控制头部存放箱是否移动位置。",
        L"代币每个可获得1分，或根据你存入的数量乘算。",
        L"每次自杀时，此选项会扣除你1分。",
        L"每次死亡时，此选项会扣除你1分。",
        L"决定当另一个玩家占用存放箱时，玩家是否无法得分。",
        L"该设置使携带头部的玩家跑得比其他玩家快或慢。",
        L"设置玩家一次能携带多少个头。"
    },
    // Portuguese
    {
        L"Controla se a caixa de depósito das cabeças muda de posição.",
        L"Os tokens podem valer 1 ponto cada ou ser multiplicados pela quantidade depositada.",
        L"Esta opção subtrai um ponto da sua pontuação cada vez que você comete suicídio.",
        L"Esta opção subtrai um ponto da sua pontuação cada vez que você morre.",
        L"Determina se os jogadores não podem marcar pontos quando outro jogador ocupa a caixa.",
        L"Esta configuração faz com que os transportadores de cabeças corram mais rápido ou mais devagar que os outros jogadores.",
        L"Define quantas cabeças um jogador pode carregar de cada vez."
    }
};

const static wchar_t* g_multiplayer_variant_interface_headhunter_speed_with_head_strings[k_language_count][k_ctf_engine_player_speed_count] =
{
    // English
    {
        L"Slow",
        L"Normal",
        L"Fast"
    },
    // Japanese
    {
        L"遅い",
        L"普通",
        L"速い"
    },
    // German
    {
        L"Langsam",
        L"Normal",
        L"Schnell"
    },
    // French
    {
        L"Lent",
        L"Normal",
        L"Rapide"
    },
    // Spanish
    {
        L"Lento",
        L"Normal",
        L"Rápido"
    },
    // Italian
    {
        L"Lento",
        L"Normale",
        L"Veloce"
    },
    // Korean
    {
        L"느림",
        L"보통",
        L"빠름"
    },
    // Chinese
    {
        L"慢",
        L"正常",
        L"快"
    },
    // Portuguese
    {
        L"Lento",
        L"Normal",
        L"Rápido"
    }
};

const static wchar_t* g_multiplayer_variant_interface_headhunter_max_heads_carried_strings[k_language_count][k_headhunter_max_heads_carried_count] =
{
    // English
    {
        L"None",
        L"1",
        L"5",
        L"10"
    },
    // Japanese
    {
        L"なし",
        L"1",
        L"5",
        L"10"
    },
    // German
    {
        L"Keine",
        L"1",
        L"5",
        L"10"
    },
    // French
    {
        L"Aucun",
        L"1",
        L"5",
        L"10"
    },
    // Spanish
    {
        L"Ninguno",
        L"1",
        L"5",
        L"10"
    },
    // Italian
    {
        L"Nessuno",
        L"1",
        L"5",
        L"10"
    },
    // Korean
    {
        L"없음",
        L"1",
        L"5",
        L"10"
    },
    // Chinese
    {
        L"无",
        L"1",
        L"5",
        L"10"
    },
    // Portuguese
    {
        L"Nenhum",
        L"1",
        L"5",
        L"10"
    }
};
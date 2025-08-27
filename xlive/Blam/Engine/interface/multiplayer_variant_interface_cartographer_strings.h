// ReSharper disable StringLiteralTypo

#pragma once
#include "multiplayer_variant_settings_interface_definition.h"
#include "main/game_preferences.h"

const static wchar_t* g_multiplayer_variant_interface_cartographer_title_strings[k_language_count][k_variant_setting_parameter_type_cartographer_count] =
{
    // English
    {
        L"Engine mode",
        L"Infinite Ammo",
        L"Infinite Grenades",
        L"Explosion Physics",
        L"Force Default FoV",
        L"Force Default Weapon Offset",
        L"Force Default Crosshair Offset",
        L"Game Speed",
        L"Gravity",
        L"Spawn Protection"
    },
    // Japanese
    {
        L"エンジンモード",
        L"無限弾薬",
        L"無限手榴弾",
        L"爆発物理",
        L"デフォルト視野を強制",
        L"デフォルトの武器オフセットを強制",
        L"デフォルトの照準オフセットを強制",
        L"ゲーム速度",
        L"重力",
        L"スポーン保護"
    },
    // German
    {
        L"Engine-Modus",
        L"Unendliche Munition",
        L"Unendliche Granaten",
        L"Explosionsphysik",
        L"Standard-Sichtfeld erzwingen",
        L"Standard-Waffenversatz erzwingen",
        L"Standard-Zielkreuzversatz erzwingen",
        L"Spielgeschwindigkeit",
        L"Schwerkraft",
        L"Spawn-Schutz"
    },
    // French
    {
        L"Mode moteur",
        L"Munitions infinies",
        L"Grenades infinies",
        L"Physique des explosions",
        L"Forcer le FOV par défaut",
        L"Forcer le décalage d'arme par défaut",
        L"Forcer le décalage du réticule par défaut",
        L"Vitesse du jeu",
        L"Gravité",
        L"Protection de réapparition"
    },
    // Spanish
    {
        L"Modo de motor",
        L"Munición infinita",
        L"Granadas infinitas",
        L"Física de explosiones",
        L"Forzar FOV predeterminado",
        L"Forzar compensación de arma predeterminada",
        L"Forzar compensación de mira predeterminada",
        L"Velocidad del juego",
        L"Gravedad",
        L"Protección de aparición"
    },
    // Italian
    {
        L"Modalità motore",
        L"Munizioni infinite",
        L"Granate infinite",
        L"Fisica delle esplosioni",
        L"Forzare FOV predefinito",
        L"Forzare offset arma predefinito",
        L"Forzare offset mirino predefinito",
        L"Velocità di gioco",
        L"Gravità",
        L"Protezione allo spawn"
    },
    // Korean
    {
        L"엔진 모드",
        L"무한 탄약",
        L"무한 수류탄",
        L"폭발 물리",
        L"기본 시야각 강제",
        L"기본 무기 오프셋 강제",
        L"기본 조준선 오프셋 강제",
        L"게임 속도",
        L"중력",
        L"스폰 보호"
    },
    // Chinese
    {
        L"引擎模式",
        L"无限弹药",
        L"无限手榴弹",
        L"爆炸物理",
        L"强制默认视野",
        L"强制默认武器偏移",
        L"强制默认准星偏移",
        L"游戏速度",
        L"重力",
        L"出生保护"
    },
    // Portuguese
    {
        L"Modo de motor",
        L"Munição infinita",
        L"Granadas infinitas",
        L"Física de explosões",
        L"Forçar o FOV padrão",
        L"Forçar o deslocamento de arma padrão",
        L"Forçar o deslocamento de mira padrão",
        L"Velocidade do jogo",
        L"Gravidade",
        L"Proteção de spawn"
    }
};


const static wchar_t* g_multiplayer_variant_interface_cartographer_parameter_description_strings[k_language_count][k_variant_setting_parameter_type_cartographer_count] =
{
    // English
    {
        L"Changes the games internal tick rate",
        L"Gives weapons infinite reserve ammo",
        L"Grants players infinite grenades",
        L"Makes explosion physics affect all players in the lobby",
        L"Forces all players to use the games default Field of View",
        L"Forces all players to use the games default weapon positions",
        L"Forces all players to use the games default crosshair position",
        L"Changes the speed of game play",
        L"Makes things heavy or light, or weightless?",
        L"Determines how long a player has extreme damage resistance after spawning"
    },
    // Japanese
    {
        L"ゲームの内部ティックレートを変更します",
        L"武器に無限の予備弾薬を与えます",
        L"プレイヤーに無限の手榴弾を付与します",
        L"ロビー内の全プレイヤーに爆発物理を影響させます",
        L"全プレイヤーにゲームのデフォルト視野を使用させます",
        L"全プレイヤーにゲームのデフォルト武器位置を使用させます",
        L"全プレイヤーにゲームのデフォルト照準位置を使用させます",
        L"ゲームプレイの速度を変更します",
        L"物体を重くしたり軽くしたり、無重力にしますか？",
        L"スポーン後にプレイヤーが極端なダメージ耐性を持つ時間を決定します"
    },
    // German
    {
        L"Ändert die interne Tickrate des Spiels",
        L"Gibt den Waffen unendliche Reservemunition",
        L"Ermöglicht Spielern unendliche Granaten",
        L"Lässt die Explosionsphysik alle Spieler in der Lobby beeinflussen",
        L"Erzwingt, dass alle Spieler das standardmäßige Sichtfeld des Spiels verwenden",
        L"Erzwingt, dass alle Spieler die Standard-Waffenpositionen des Spiels verwenden",
        L"Erzwingt, dass alle Spieler die Standard-Zielkreuzposition des Spiels verwenden",
        L"Ändert die Spielgeschwindigkeit",
        L"Macht Dinge schwer oder leicht oder schwerelos?",
        L"Bestimmt, wie lange ein Spieler nach dem Spawnen extremen Schadenswiderstand hat"
    },
    // French
    {
        L"Modifie le taux de tick interne du jeu",
        L"Donne aux armes des munitions de réserve infinies",
        L"Accorde aux joueurs des grenades infinies",
        L"Fait en sorte que la physique des explosions affecte tous les joueurs dans le lobby",
        L"Force tous les joueurs à utiliser le champ de vision par défaut du jeu",
        L"Force tous les joueurs à utiliser les positions d'arme par défaut du jeu",
        L"Force tous les joueurs à utiliser la position de réticule par défaut du jeu",
        L"Modifie la vitesse de jeu",
        L"Rend les objets lourds ou légers, ou en apesanteur ?",
        L"Détermine la durée de la résistance aux dégâts extrêmes d'un joueur après son apparition"
    },
    // Spanish
    {
        L"Cambia la tasa de ticks interna del juego",
        L"Proporciona munición de reserva infinita a las armas",
        L"Otorga a los jugadores granadas infinitas",
        L"Hace que la física de explosiones afecte a todos los jugadores en el lobby",
        L"Fuerza a todos los jugadores a usar el campo de visión predeterminado del juego",
        L"Fuerza a todos los jugadores a usar las posiciones de arma predeterminadas del juego",
        L"Fuerza a todos los jugadores a usar la posición de retícula predeterminada del juego",
        L"Cambia la velocidad del video juego",
        L"¿Hace que las cosas sean pesadas o ligeras, o ingrávidas?",
        L"Determina cuánto tiempo tiene un jugador resistencia extrema al daño después de aparecer"
    },
    // Italian
    {
        L"Modifica il tasso di tick interno del gioco",
        L"Dà alle armi munizioni di riserva infinite",
        L"Concede ai giocatori granate infinite",
        L"Fa sì che la fisica delle esplosioni influenzi tutti i giocatori nella lobby",
        L"Costringe tutti i giocatori a usare il campo visivo predefinito del gioco",
        L"Costringe tutti i giocatori a usare le posizioni delle armi predefinite del gioco",
        L"Costringe tutti i giocatori a usare la posizione del mirino predefinita del gioco",
        L"Modifica la velocità di gioco",
        L"Rende gli oggetti pesanti o leggeri, o senza peso?",
        L"Determina per quanto tempo un giocatore ha resistenza estrema ai danni dopo lo spawn"
    },
    // Korean
    {
        L"게임의 내부 틱 레이트를 변경합니다",
        L"무기에 무한 예비 탄약을 제공합니다",
        L"플레이어에게 무한 수류탄을 부여합니다",
        L"로비의 모든 플레이어에게 폭발 물리 효과를 적용합니다",
        L"모든 플레이어가 게임의 기본 시야를 사용하도록 강제합니다",
        L"모든 플레이어가 게임의 기본 무기 위치를 사용하도록 강제합니다",
        L"모든 플레이어가 게임의 기본 조준선 위치를 사용하도록 강제합니다",
        L"게임 플레이 속도를 변경합니다",
        L"무거움, 가벼움 또는 무중력을 적용합니까?",
        L"스폰 후 플레이어가 극심한 피해 저항을 가지는 시간을 결정합니다"
    },
    // Chinese
    {
        L"更改游戏的内部更新速率",
        L"为武器提供无限的备用弹药",
        L"给予玩家无限的手榴弹",
        L"让爆炸物理效果影响大厅中的所有玩家",
        L"强制所有玩家使用游戏的默认视野",
        L"强制所有玩家使用游戏的默认武器位置",
        L"强制所有玩家使用游戏的默认准星位置",
        L"更改游戏速度",
        L"使物体变重或变轻，或失重？",
        L"确定玩家出生后获得极高伤害抗性的持续时间"
    },
    // Portuguese
    {
        L"Altera a taxa interna de ticks do jogo",
        L"Dá às armas munição de reserva infinita",
        L"Concede aos jogadores granadas infinitas",
        L"Faz com que a física de explosão afete todos os jogadores no lobby",
        L"Força todos os jogadores a usarem o campo de visão padrão do jogo",
        L"Força todos os jogadores a usarem as posições de arma padrão do jogo",
        L"Força todos os jogadores a usarem a posição de mira padrão do jogo",
        L"Altera a velocidade do jogo",
        L"Torna as coisas pesadas ou leves, ou sem peso?",
        L"Determina quanto tempo um jogador tem resistência extrema a danos após o aparecimento"
    }
};


const static wchar_t* g_multiplayer_variant_interface_cartographer_game_speed_strings[k_language_count][k_game_speed_modifier_count] =
{
    // English
    {
        L"Default",
        L"50%% Speed",
        L"150%% Speed",
        L"200%% Speed",
        L"It's time to stop!"
    },
    // Japanese
    {
        L"デフォルト",
        L"50%%の速度",
        L"150%%の速度",
        L"200%%の速度",
        L"もうやめる時間だ！"
    },
    // German
    {
        L"Standard",
        L"50%% Geschwindigkeit",
        L"150%% Geschwindigkeit",
        L"200%% Geschwindigkeit",
        L"Jetzt ist Schluss!"
    },
    // French
    {
        L"Par défaut",
        L"Vitesse 50%%",
        L"Vitesse 150%%",
        L"Vitesse 200%%",
        L"Il est temps d'arrêter !"
    },
    // Spanish
    {
        L"Predeterminado",
        L"Velocidad 50%%",
        L"Velocidad 150%%",
        L"Velocidad 200%%",
        L"¡Es hora de parar!"
    },
    // Italian
    {
        L"Predefinito",
        L"Velocità 50%%",
        L"Velocità 150%%",
        L"Velocità 200%%",
        L"È ora di fermarsi!"
    },
    // Korean
    {
        L"기본",
        L"50%% 속도",
        L"150%% 속도",
        L"200%% 속도",
        L"멈출 시간이다!"
    },
    // Chinese
    {
        L"默认",
        L"50%% 速度",
        L"150%% 速度",
        L"200%% 速度",
        L"该停下来了！"
    },
    // Portuguese
    {
        L"Padrão",
        L"Velocidade 50%%",
        L"Velocidade 150%%",
        L"Velocidade 200%%",
        L"É hora de parar!"
    }
};

const static wchar_t* g_multiplayer_variant_interface_cartographer_gravity_strings[k_language_count][k_game_gravity_modifier_count] =
{
    // English
    {
        L"Default",
        L"25%% Gravity",
        L"50%% Gravity",
        L"75%% Gravity",
        L"125%% Gravity",
        L"150%% Gravity",
        L"175%% Gravity",
        L"200%% Gravity"
    },
    // Japanese
    {
        L"デフォルト",
        L"重力25%%",
        L"重力50%%",
        L"重力75%%",
        L"重力125%%",
        L"重力150%%",
        L"重力175%%",
        L"重力200%%"
    },
    // German
    {
        L"Standard",
        L"25%% Schwerkraft",
        L"50%% Schwerkraft",
        L"75%% Schwerkraft",
        L"125%% Schwerkraft",
        L"150%% Schwerkraft",
        L"175%% Schwerkraft",
        L"200%% Schwerkraft"
    },
    // French
    {
        L"Par défaut",
        L"Gravité 25%%",
        L"Gravité 50%%",
        L"Gravité 75%%",
        L"Gravité 125%%",
        L"Gravité 150%%",
        L"Gravité 175%%",
        L"Gravité 200%%"
    },
    // Spanish
    {
        L"Predeterminado",
        L"Gravedad 25%%",
        L"Gravedad 50%%",
        L"Gravedad 75%%",
        L"Gravedad 125%%",
        L"Gravedad 150%%",
        L"Gravedad 175%%",
        L"Gravedad 200%%"
    },
    // Italian
    {
        L"Predefinito",
        L"Gravità 25%%",
        L"Gravità 50%%",
        L"Gravità 75%%",
        L"Gravità 125%%",
        L"Gravità 150%%",
        L"Gravità 175%%",
        L"Gravità 200%%"
    },
    // Korean
    {
        L"기본",
        L"중력 25%%",
        L"중력 50%%",
        L"중력 75%%",
        L"중력 125%%",
        L"중력 150%%",
        L"중력 175%%",
        L"중력 200%%"
    },
    // Chinese
    {
        L"默认",
        L"25%% 重力",
        L"50%% 重力",
        L"75%% 重力",
        L"125%% 重力",
        L"150%% 重力",
        L"175%% 重力",
        L"200%% 重力"
    },
    // Portuguese
    {
        L"Padrão",
        L"Gravidade 25%%",
        L"Gravidade 50%%",
        L"Gravidade 75%%",
        L"Gravidade 125%%",
        L"Gravidade 150%%",
        L"Gravidade 175%%",
        L"Gravidade 200%%"
    }
};

const static wchar_t* g_multiplayer_variant_interface_cartographer_spawn_protection_strings[k_language_count][k_player_spawn_protection_timer_count] =
{
    // English
    {
        L"Default",
        L"None",
        L"3 Seconds",
        L"5 Seconds",
        L"10 Seconds"
    },
    // Japanese
    {
        L"デフォルト",
        L"0秒",
        L"3秒",
        L"5秒",
        L"10秒"
    },
    // German
    {
        L"Standard",
        L"0 Sekunde",
        L"3 Sekunden",
        L"5 Sekunden",
        L"10 Sekunden"
    },
    // French
    {
        L"Par défaut",
        L"0 seconde",
        L"3 secondes",
        L"5 secondes",
        L"10 secondes"
    },
    // Spanish
    {
        L"Predeterminado",
        L"0 segundo",
        L"3 segundos",
        L"5 segundos",
        L"10 segundos"
    },
    // Italian
    {
        L"Predefinito",
        L"0 secondo",
        L"3 secondi",
        L"5 secondi",
        L"10 secondi"
    },
    // Korean
    {
        L"기본",
        L"0초",
        L"3초",
        L"5초",
        L"10초"
    },
    // Chinese
    {
        L"默认",
        L"0秒",
        L"3秒",
        L"5秒",
        L"10秒"
    },
    // Portuguese
    {
        L"Padrão",
        L"0 segundo",
        L"3 segundos",
        L"5 segundos",
        L"10 segundos"
    }
};

const static wchar_t* g_multiplayer_variant_interface_cartographer_engine_mode[k_language_count][2] =
{
    // English
    {
        L"Default",
        L"Legacy"
    },
    // Japanese
    {
        L"デフォルト",
        L"レガシー"
    },
    // German
    {
        L"Standard",
        L"Legacy"
    },
    // French
    {
        L"Par défaut",
        L"Legacy"
    },
    // Spanish
    {
        L"Predeterminado",
        L"Legado"
    },
    // Italian
    {
        L"Predefinito",
        L"Legacy"
    },
    // Korean
    {
        L"기본",
        L"레거시"
    },
    // Chinese
    {
        L"默认",
        L"传统"
    },
    // Portuguese
    {
        L"Padrão",
        L"Legado"
    }
};
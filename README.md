# AviUtl 右クリメニューショトカ追加プラグイン

拡張編集の各種右クリックメニューに，アクセラレータキー（ショートカットキー）を追加するプラグイン．各メニュー項目の名前の末尾に `(&X)` の形の文字列を付加します．

拡張編集フィルタプラグイン (`.eef`) や[アルティメットプラグイン](https://github.com/hebiiro/anti.aviutl.ultimate.plugin)の[フィルタコピー](https://hebiiro.github.io/anti.aviutl.ultimate.plugin/contents/addin/filter_copy.html)で追加されたメニュー項目などにもショートカットを追加できます．

![アクセラレータキーの例1](https://github.com/user-attachments/assets/03e2e3ab-59e7-4cfa-a891-e11c60c1871f) ![アクセラレータキーの例2](https://github.com/user-attachments/assets/99e750e8-770f-4e6d-9489-b386ace64f47) ![アルティメットプラグインとも共存可能](https://github.com/user-attachments/assets/b8e3ca5c-494b-415d-8363-cd9da772af5d)

[ダウンロードはこちら．](https://github.com/sigma-axis/aviutl_rclick_accel/releases) \[紹介動画準備中...\]

## 動作要件

- AviUtl 1.10 + 拡張編集 0.92

  http://spring-fragrance.mints.ne.jp/aviutl

  - 拡張編集 0.93rc1 等の他バージョンでは動作しません．

- Visual C++ 再頒布可能パッケージ（\[2015/2017/2019/2022\] の x86 対応版が必要）

  https://learn.microsoft.com/ja-jp/cpp/windows/latest-supported-vc-redist

## 導入方法

以下のフォルダのいずれかに `rclick_accel.auf` と `rclick_accel.ini` をコピーしてください．

1. `aviutl.exe` のあるフォルダ
1. (1) のフォルダにある `plugins` フォルダ
1. (2) のフォルダにある任意のフォルダ

## 使い方

AviUtl を起動する前に `rclick_accel.ini` をテキストエディタで編集して，どの右クリックメニューのどの項目にどのキーを割り当てるのかを指定してください．

右クリックメニューの表示箇所ごとに `[timeline_blank]` のように記述が分かれています．対応した右クリックメニュー内の項目ごとにショートカットキーを個別に設定できます．

`rclick_accel.ini` ファイルを保存したのちに AviUtl を起動すれば，各種右クリックメニューの項目末尾に `(&X)` の形の文字列が付加され，ショートカットキーとして機能します．

### `.ini` ファイルの記述

以下のように右クリックメニューが分類されています:

1.  `[timeline_blank]`

    タイムラインの何もないところを右クリックしたときのメニューです．

    ![タイムラインの何もないところを右クリックしたときのメニュー](https://github.com/user-attachments/assets/15dc4cf3-b1fb-40be-b66c-0953ac7751e9)

    - `メディアオブジェクトの追加`, `フィルタオブジェクトの追加` のサブメニュー配下の項目は設定の対象外で，代わりに `[filters]` で指定した設定が適用されます．

1.  `[timeline_object]`

    タイムラインに配置されたオブジェクトを右クリックしたときのメニューです．

    ![タイムラインに配置されたオブジェクトを右クリックしたときのメニュー](https://github.com/user-attachments/assets/105775ff-30e9-4dbd-9bd7-2cf906601b82)

1.  `[layer]`

    タイムライン左のレイヤー部分を右クリックしたときのメニューです．

    ![タイムライン左のレイヤー部分を右クリックしたときのメニュー](https://github.com/user-attachments/assets/4075e70a-0b2b-484a-97ab-9037074c40d5)

1.  `[scene]`

    タイムライン左上のシーンボタン付近を右クリックしたときのメニューです．

    ![タイムライン左上のシーンボタン付近を右クリックしたときのメニュー](https://github.com/user-attachments/assets/f34b456e-9c49-44de-b268-7b4bd23148c0)

    - 「シーンの設定」の1項目のみ．

1.  `[setting_dialog]`

    設定ダイアログを右クリックしたときのメニューです．

    ![設定ダイアログを右クリックしたときのメニュー](https://github.com/user-attachments/assets/943b87e5-fa1c-4f37-8397-25a5c6b6c948)

    - `フィルタ効果の追加` のサブメニュー配下の項目は設定の対象外で，代わりに `[filters]` で指定した設定が適用されます．

    [アルティメットプラグイン](https://github.com/hebiiro/anti.aviutl.ultimate.plugin)の[フィルタコピー](https://hebiiro.github.io/anti.aviutl.ultimate.plugin/contents/addin/filter_copy.html)などでメニュー項目が追加されている場合，ここに追記することでそのメニュー項目にもショートカットキーが割り当てられます．

    - 記述例:

      ```ini
      このフィルタを切り取り = X
      このフィルタをコピー = C
      フィルタを貼り付け = V
      ```

      ![結果の例](https://github.com/user-attachments/assets/b8e3ca5c-494b-415d-8363-cd9da772af5d)

1.  `[setting_dialog_interval]`

    設定ダイアログ上部の中間点配置などの表示を右クリックしたときのメニューです．

    ![設定ダイアログ上部の中間点配置などの表示を右クリックしたときのメニュー](https://github.com/user-attachments/assets/ade404d9-94aa-4c0c-a07e-b1c56a7e5699)

1.  `[filters]`

    各種フィルタ効果や入出力フィルタを選択するメニューです．

    ![各種フィルタ効果や入出力フィルタを選択するメニューの1つ](https://github.com/user-attachments/assets/bf4dde6c-f2d5-4aba-9621-9eb49647695a)

    - タイムラインの右クリックでの `メディアオブジェクトの追加`, `フィルタオブジェクトの追加` や，設定ダイアログでの `フィルタ効果の追加` 内に含まれる項目が対象です．

    - 設定ダイアログ右上の :arrows_counterclockwise: や :heavy_plus_sign: のボタンを押したときのメニュー項目にも適用されます．

    登録済みエイリアスや，拡張編集フィルタプラグイン (`.eef` 形式) で追加されたフィルタ効果がある場合，ここに追記することでそのエイリアスやフィルタ効果のメニュー項目にもショートカットキーを割り当てられます:

    - [円形縁取りプラグイン](https://github.com/sigma-axis/aviutl_CircleBorder_S)を導入した場合の例:

      ```ini
      縁取りσ = B
      角丸めσ = R
      アウトラインσ = O
      ```

      ![結果の例](https://github.com/user-attachments/assets/7e6341e4-e329-4c96-99b8-5fdbc41fdc4c)

1.  `[easing_settings]`

    トラックバー変化方法を選ぶメニューの，各種設定部分です．この部分は次の3項目のみが使われます (他の項目を追加しても使用されません).

    1.  `加速`
    1.  `減速`
    1.  `設定`

    ![トラックバー変化方法を選ぶメニュー](https://github.com/user-attachments/assets/54aab5f2-db57-44c1-a497-e2afd70603e9)

1.  `[easings]`

    トラックバー変化方法を選ぶメニューの，一覧部分です．

    ![トラックバー変化方法を選ぶメニュー](https://github.com/user-attachments/assets/54aab5f2-db57-44c1-a497-e2afd70603e9)

    外部スクリプトの導入などでトラックバーの変化方法が追加されている場合は，ここに追記することでその変化方法のメニュー項目にもショートカットキーを割り当てられます．

### 使用できるキー

ショートカットキーとして記述できる文字は ASCII コード `0x2a` から`0x7e` までの以下の文字です．記述した最初の1文字のみが読み取られ，残りは無視されます．大文字小文字の違いも無視されます．

```
* + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \ ] ^ _ ` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~
```

- これらの文字は，このプラグインが有効だと認識するというだけのものです．OS の仕様などにより必ずしもショートカットキーとして使えるとは限りません．


## 既知の問題

1.  [エフェクト並び替えプラグイン](https://github.com/nazonoSAUNA/SortEffect)と併用する場合，`[filters]` 以下で `その他` に対してショートカットキーを設定するのは避けてください．設定している場合，新しく追加・作成したエイリアスが AviUtl を再起動するまでメニューに追加されなくなります．


## 改版履歴

- **v1.00** (2025-02-04)

  - 初版．


## ライセンス

このプログラムの利用・改変・再頒布等に関しては MIT ライセンスに従うものとします．

---

The MIT License (MIT)

Copyright (C) 2025 sigma-axis

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

https://mit-license.org


##  aviutl_exedit_sdk

https://github.com/ePi5131/aviutl_exedit_sdk （利用したブランチは[こちら](https://github.com/sigma-axis/aviutl_exedit_sdk/tree/self-use)です．）

---

1条項BSD

Copyright (c) 2022
ePi All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY ePi “AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ePi BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


# 連絡・バグ報告

- GitHub: https://github.com/sigma-axis
- Twitter: https://x.com/sigma_axis
- nicovideo: https://www.nicovideo.jp/user/51492481
- Misskey.io: https://misskey.io/@sigma_axis
- Bluesky: https://bsky.app/profile/sigma-axis.bsky.social

# ai_badge
## ハードウェア
- Arduino MKR WiFi 1010
- 電池ボックス (モータ用)
- モバイルバッテリー (回路用)

## 導入方法
### ハードの準備
1. 単三電池×4が入った電池ボックスを回路に接続する (回路上の－マークと黒ケーブルを合わせる）
2. 回路上にあるモータ用のスライドスイッチをONにする（Arduinoに近い方がON）  
    ※終了時は、必ずスイッチをOFFにするか、電池ボックスのケーブルを抜く

### ソフトの書き込み＆デバッグ
1. Arduino 公式ガイドに沿って、プログラムを書き込むところまで進める(Desktop IDEを推奨)  
	https://www.arduino.cc/en/Guide/MKRWiFi1010
2. ソースコードをcloneする  
	https://github.com/tdj-lomf/ai_badge
3. Desktop IDEでコードを開く(ai_badge.ino)
4. タブからWebServer.inoを開き、ssidとpassを使用するWiFi環境に合わせて書き換える
5. ArduinoとPCをmicro-usbケーブルで接続し、コードを書き込む  
	※充電専用ケーブルでは書き込めない。通信可能なケーブルを使う。
6. 10秒ほど待ち、IDE下部のSerial MonitorにIPアドレスが表示されればOK。また、このとき瞳やまぶたが初期位置に戻り、回路上の橙色のLEDが点灯する。
7. 同じWifiに接続されているPCやスマホのWebブラウザ(Chromeで動作確認済）から、表示されたIPアドレスにアクセスする。
8. 表示されたサイト上のボタンで瞳やまぶたを操作する。

### 実使用
1. PCではなく、モバイルバッテリーとArduinoをmicro-usbケーブルで接続し給電する
2. 上記と同様に10秒ほど待ち、同じIPアドレスに接続して動作確認する

## コーディング

### モータ動作
- MoveComand.inoに動作パターン（コマンド）を追加していく。
    - 瞳の動きならeyeCommand, まぶたの動きならeyelidCommandに追加する。
    - 瞳とまぶたが同時に動く必要がある場合は未作成なので、どちらかに追加するか、第三の関数を追加するかしないといけない。
- モータを動かす関数群は、EyeMove.inoに書く。
    - moveEye, moveEyelid: モータを動かす最も基本的な関数。可能な限り早く指定位置に動かす。
    - moveEyeP, moveEyelidP: 上記のパーセント指定バージョン。
    - moveEyeSync, moveEyelidSync: 動作時間を指定できるバージョン。その時間まで他の処理ができなくなる点に注意。
    - moveEyeDiff, moveEyelidDiff: Syncの相対位置バージョン。現在位置からどれだけ動かすかをパーセントで指定する。

### LED
LEDのON/OFFは下記のように行う。
```c++
  digitalWrite(LED1, HIGH);  // ON
  digitalWrite(LED1, LOW);  // OFF
```

ラベルと実物の対応  

| ラベル | 実物 |
| ---- | ---- |
| LED1 | Arduinoに近い側に接続したLED |
| LED2 | Arduinoに遠い側に接続したLED |
| LED3 | Arduino上の橙色のLED |
    
## トラブルシューティング
- Webブラウザとの通信が切れた
  - 回路上のリセットボタンを押し、再び接続されるまで待つ
  - 切断原因不明。自動的にリセットできるように修正できるか？
- 瞳が動かなくなった
  - リンクが特異点を越えたり、外れた可能性がある。瞳を左回しで外し、ケースの2箇所のネジを取って、中を確認する。
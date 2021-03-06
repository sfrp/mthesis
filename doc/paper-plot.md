マイクロコントローラのための実用的な関数リアクティブプログラミング言語
====================================================

# 論文梗概
* 最後に書く

---
# 序論
## 研究の背景と目的
* 非同期的に発生するイベントに反応し何らかのを処理を行うプログラムは、組込みシステムの典型である。
    * 小規模な組込みシステムにおいても同様である。
* こういった処理は並行処理の機構を持たない手続き的なプログラミング言語においては簡潔に表現しづらい。
    * 入力値の取得と出力値の更新を順に行う処理をループさせることによって表現するのが一般的であるが、冗長で複雑な記述になりがちである。
* 割り込みやソフトウェアスレッドなどを利用してコールバックの機構を実現するという手法もあるが、あまり簡潔な記述は得られない。
    * コンポーネント化が難しく、複数の処理を組み合わせづらい。
    * 実行系列が非同期的に移り変わることとなるため、却ってプログラムの見通しが悪くなることも多い。
* こういった処理を簡潔に記述する方法として、現在ではFRPやRP等と呼ばれる手法が広く利用されるようになってきた。
    * Webフロントエンド向けのElm言語やReduxフレームワーク、様々な言語向けに開発されているReactiveXライブラリなど。
* しかし、従来研究されてきたFRPやRPの手法を、マイクロコントローラを始めとする制約的環境において用いることは難しい。
    * FRPやRPの手法が普及している、Java等のメモリを多く消費する言語をマイクロコントローラプログラミングに用いることは難しい。
    * C++をサポートするマイクロコントローラも存在するが、RxCpp等の巨大なRPライブラリをマイクロコントローラにおいて利用することは難しい。
* 本研究では、マイクロコントローラにおいて実行可能な独自のFRPの手法を考案し、その手法に則った軽量なFRP言語を設計する。
    * 従来のFRP手法から、小規模組込みシステム開発に不要あるいは不適な表現に対して制限を加え、単純化を図る。
    * メモリを多く消費せず、また動的なメモリ確保も行わずに実行可能なFRPの計算モデルを考案する。
* この軽量FRP言語のコンパイラを実装する。
    * C言語を中間表現とするコンパイラである。
    * 実行性能について簡単な評価を行う。
* また、実例研究を行うと共に、本FRP言語の記述性能を分析する。
    * 例題となる組込みシステムを示し、これを本FRP言語によって実装する。
    * 本FRP言語が持つ機能の、記述性や実用性に与える影響について分析を行う。

## 本論文の構成
* 最後に書く

---
# 背景知識
## 関数リアクティブプログラミング
* 入出力の相互作用を関数として表現するプログラミング手法を、関数リアクティブプログラミング（Functional Reactive Programming, FRP)と呼ぶ。
    * FRPでは、時刻に依存して変動する値をtime-varying value(時変値)として抽象化する。
    * 時変値同士を合成することによって新たな時変値を定義することができる。
    * アプリケーションの入力値を外部から与えられる時変値と見做せば、アプリケーションの出力値もまた時変値として導出することができる。
* FRPは1997年にConal Elliottらによって提唱され、彼らによって初のFRPフレームワークFranが開発された。
    * 純粋関数型言語であるHaskellにおける内部DSLとして実装された。
* Franは表現の自由度こそ高かったものの、リアルタイム性に問題があった。
    * memory-leakやtime-leak(計算量の増大)を誘発する問題を抱えていた。
* Franが抱える問題を解消するための研究が行われ、新たなFRPフレームワークの開発が行われた。
    * RT-FRP, Arrowized-FRP, Event-Driven-FRP
    * いずれもFranと同様にHaskellを対象として実装された。
* 2009年にはリアクティブプログラミング(RP)のためのライブラリReactiveX(Reactive Extensions, Rx)がMicrosoftによって開発された。
    * 当初は.NET Frameworkに向けて作られたRx.NETのみが提供されたが、現在では様々な言語向けに移植されて開発が続けられている。
    * ReactiveXは参照透明性こそ保証しないが、抽象化の手法や目的は過去に研究されてきたFRPに近いものがあり、広い解釈ではFRPライブラリであると見なせる。
* 2012年にはWebフロントエンドアプリケーションの記述に特化したFRP言語ElmがEvan Czaplickiによって開発された。
    * 過去のFRPの研究の流れを汲む形で単純化され、Haskellに依らない独自の構文と意味論を伴うプログラミング言語として設計された。

## マイクロコントローラと小規模組込みシステム
* コンピュータ・システムを一つの集積回路としてコンパクトに纏めたものを、マイクロコントローラ(microcontroller)と呼ぶ。
    * CPUやメモリ、入出力回路やタイマー回路などが一纏めにされており、電源を繋ぐだけで単体で動作可能なものも多い。
    * 現在では家電を始めとする電子機器の大半にマイクロコントローラが搭載され、その機能制御を担っている。
* 様々な用途向けのマイクロコントローラが製品化されており、製品によってその価格や性能は様々である。
* 低価格帯のマイクロコントローラの多くは数KBのFlashとその1/10程度のRAMを持つ程度であり、豊富なメモリ資源を持たない。
* アセンブリ言語を用いる程ではないにしろ、C++の高レベルな機能を利用してプログラミングを行うのが難しい場合は多い。
    * C++によって記述されたライブラリをリンクすると、プログラムサイズ大きくなりがちである。
    * そもそもC++コンパイラが提供されていない場合も多い。
* 現代における小規模組込みシステム開発ではC言語が主力となる。
    * 主要なマイクロコントローラでは大抵の場合においてC言語コンパイラが提供されている。
    * 言語機能が低レベルであるが故に、メモリ使用量の見積もりが容易である。
* 動的にメモリを確保することは組込みシステム開発において避けるべきである。
    * プログラムの動作中にメモリが枯渇してしまうと、プログラムを続行することが困難になり、機器が制御不能に陥る。
    * 関数の再帰呼出しはスタック領域におけるメモリの動的確保であると言え、これも避けるべきである。
    * C++のSTLを始めとする、暗黙的にメモリの動的確保を行うライブラリには注意が必要である。
* 電子機器の中で独立して動作するプログラムにとって、プログラムの品質の確保は非常に大切である。
    * バグを作りこまないために、記述性やテスト可能性を確保することが大切である。

---
# 言語デザイン
* 本研究では、記述性と省メモリ実装性を重視した組込みシステム記述用FRP言語SFRPを設計する。

## FRPに基づく独自の計算モデル
* 本節では、SFRPが採用するFRPの計算モデルを示す。
* 通常の値と時変値は区別されず、プログラム中の全ての値は時変値であると見なされる。
* プログラムへの入力は、入力値に対応するプリミティブな時変値として表現される。
* 他の時変値を参照し、演算を加える時変値を新たに生成することが可能である。
    * たとえば、時変値Aの値と時変値Bの値の和を取る時変値Cを生成する、といったことが可能である。
* 各時変値は、現在時刻tにおける値(現在値)の参照と、微小時間Δt前の値(直前値)の参照が許されている。
    * ただし、直前値の参照が許されるのは、初期値が指定された時変値のみである。
    * tおよびΔtは抽象値であり、Δtはtに依存して決まる。
* プログラムの出力は任意の時変値を出力として指定することによって表現される。

## 言語の構文と意味
* 本節では、SFRPの構文とその意味を示す。
    * 外部ノード定義
    * 内部ノード定義
    * ノード出力定義
    * 関数定義
    * 外部関数定義
    * 式

## メモリ使用量の評価
* 組込みシステム開発では多くの場合、プログラムが消費するメモリ使用量を何らかの形で見積もる必要がある。
* SFRPでは、プログラムがデータ構造を保持するするために必要な最大メモリ量を評価できるように設計されている。
* 以下に評価器(:: Program -> UsedMemorySize)を提示する[figure]。

---
# 実装
* 本研究ではリファレンス実装として、C言語を中間表現とするSFRPのコンパイラを作成した。
    * https://github.com/sfrp/sfrp
* 本章では、本実装によってプログラムがどのように実行されるかについて説明する。

## SFRPプログラム例
* 以降の節では、以下に示すSFRPのプログラム例を元に解説および評価を行う。
* [code]

## 実行過程
* 実行開始直後に一度だけ、全ての初期値付きノードの初期値が計算され、初回の直前値として参照される。
* 依存関係の順に従い、全てのノードの現在値が導出される。
* 初期値付きノードの現在値は次回の計算フェーズまで保持され、次回の直前値として参照される。
* プログラム例の場合、以下に示す処理が行われる。

## データ構造の取り扱い
* データ構造の実体は静的領域に確保され、ポインタを介して参照される。
* 参照されなくなったデータ構造は、新たなデータ構造を生成する際に再利用される。
* ノードの現在値演算が一巡する毎に、全てのデータ構造の参照到達性判定が行われる。
    * マーク＆スウィープ形式によって到達性が判定される。
    * このとき、初期値付きノードの現在値がルートオブジェクトに該当する。
* プログラム例の場合、以下に示す処理が行われる。

## 実行時性能
* 本実装によって実行されるプログラムがどの程度の実行時性能を発揮するのかを調査する。
* プログラム例の実行時性能を計測し、本実装の性能評価とする。
    * C言語による中間表現およびgccを用いて生成したバイナリを元に、実際に使用されるメモリの量を調査する[experiment]。
    * マイコン上でプログラムを実行し、速度に関するベンチマークを取る[experiment]。

---
# 実例研究
* (キッチン)タイマーを実例研究として作成する。

## 仕様
* 仕様を定義する。状態遷移図[figure]を載せて分り易く説明。

## ハードウェア
* ハードウェア構成の説明。回路図[figure]も載せる。プロダクトの写真も載せる。

## ソフトウェア
* ソースコード[code]を載せて、簡単に解説する。

## メモリ使用量の見積もり
* プログラムをもとに、データ構造の確保に必要なメモリ量を算出する[experiment]。
* gccを用いてバイナリを生成し、静的に確保されるメモリの量を調べる[experiment]。

---
# 記述性と実用性
* 本章では、SFRPにおける特徴的な仕組みを取り上げ、記述性と実用性に与える影響について論じる。

## 代数的データ構造とパターンマッチ
* 非再帰的な代数的データ構造を多相的に定義することができる。
* 組込みシステムの状態を簡潔に表現するのに代数的データ構造は非常に有用である。
    * 組込みシステムの状態は複数の状態変数の積和で表現されることが多いため。
* 網羅性チェック機能付きのパターンマッチ構文により、安全かつ網羅的な状態遷移処理が可能となる。

## シームレスな時変値プログラミング
* 通常の値から時変値へと変換したり、時変値から通常の値を取り出したりといった操作を必要としない。
* 値を全て時変値として扱うことによって、より簡易なFRPを実現している。
* 時変値に関する高階プログラミングのサポートは切り捨てている。
    * 小規模組込みシステムプログラミングにとって、複雑で自由度の高い言語機能は不要である場合が多いため。
* SFRPは、入出力の繁雑な依存関係に対して高い記述性を確保することに注力していると言える。

## 再帰の禁止
* SFRPでは、再帰的関数および再帰的データ構造の定義を禁止している。
* 再帰的データ構造は原理的にメモリの動的確保を必要とするため、組込みシステム開発に用いることは望ましくない。
    * どうしても再帰的データ構造が必要となる場合は考えられるが、そういった状況への直接的なサポートは行っていない。
    * 外部定義を介せば間接的に実現することは可能。
* 再帰的関数も同様にスタック領域におけるメモリの動的確保を必要とするため、組み込みシステム開発に用いることは望ましくない。
* ただし、SFRPではC言語におけるfor文のようなループ構文が存在しないため、反復処理を直接的に表現する手段を持たないことになる。
    * SFRPでは、時変値の直前値参照を介して反復的な処理を記述する必要があり、やや不便であると言える。
* このことは、リアルタイム性を保証する上ではプラスとなっている。
    * 反復処理を記述する手段がないことによって、全ての時変値の値は即時的に計算可能である(言い換えれば、時間計算量O(1)である)ことが保証される。

## 破壊的な変更の禁止
* SFRPでは、データに対する破壊的な操作を行うことはできない。
* ポインタの読み書き等の処理を行いたい場合は、外部定義を介したI/Oとして行う必要がある。
* SFRPでは、破壊的な変更を多く用いる必要のあるシステムを記述するには不向きであるといえる。

## 外部ノード定義とノード出力定義
* 外界に存在する時変値を言語内で取り扱うための構文として、外部ノード定義がある。
* 外界に存在する時変値の定義を言語内で与えるための構文として、ノード出力定義がある。
* 入力値を取得する際に他の時変値を参照することができる点は便利である。
* 出力の際に複数の時変値の値をI/O関数に与えられる点は便利である。

## 通常関数とI/O関数
* 通常の純粋関数とは別に、副作用を伴うI/O関数を定義することができる。
* 副作用を伴う処理を言語内にて記述することができる。
* 通常関数の純粋性を確保するために、通常関数内においてI/O関数を呼び出すことは禁止されている。

## 外部定義
* C言語等によって定義された関数を外部関数として呼び出す機能が言語仕様としてサポートされている。
* アーキテクチャに依存せず汎用的に入出力処理を組み込むための機構としては妥当である。
    * 外部関数を定義する手間はあるが、実際に組込みシステム開発を行う上でネックとなるほどではないと思われる。
* プリミティブ型を要素に持つタプルを返す外部定義関数を作成することができる。
    * SFRP内で定義されたデータ構造を外部で用いる唯一の手段となる。

---
# 関連研究
* Elm
    * ElmはWebアプリケーションフロントエンドのためのプログラミング言語である。
    * 現在のElmが採用するFRPの手法とは既に異なっているが、ここでは２０１２年の発表当初のElmについて述べる。
    * SFRPが採用するFRPの手法は、既存のFRP手法の中では、ElmによるFRPに比較的近い。
    * SFRPにおける初期値付き時変値は、Elmのfoldpによって生成される状態付き時変値をより単純化したものとみなすこともできる。
    * Elmはイベント駆動FRPだが、SFRPはイベント駆動FRPではない。
        * Elmはイベントの発生をトリガーとして時変値の更新を行う。
        * SFRPには時変値の更新という概念はなく、現在値と直前値を用いて時変値を定義する。
* RxCpp
    * ReactiveXライブラリのC++実装であるRxCppは非常に強力なリアクティブプログラミングツールである。
    * RxCppは、C++11以降の言語機能をフルに活用した実装である。
        * SFRPが対象とするような小規模組込みシステムの記述に適しているとは言い難い。
    * ReactiveXの実装に高レベルな言語機能は必ずしも必要ではないはずである。
    * 今後、省メモリ性を意識したネイティブ環境向けのReactiveX実装が生まれれば、小規模な組込みシステムにも活用できるかもしれない。
* EsterelとCeu
    * Esterelは、リアルタイムシステム向けの命令的・同期的なプログラミング言語である。
    * 命令の即時実行性が言語の意味論に含まれており、ノンブロッキングな処理とそうでない処理の明示的な書き分けが成される。
    * 明示的に記述されたブロッキング処理がプリエンプションのトリガーとなり、決定的な並行性が実現される。
    * CeuはEsterelの影響を強く受けた、組込みシステム向けのプログラミング言語である。
    * 基本的なコンセプトはEsterelと同様であるが、独自の機構を導入することで記述性やGUIプログラミング等への適合性を高めている。
    * 決定的な並行性を実現するという点はFRPと共通するが、FRPはその純粋性(参照透明性)故に同期的実行と非同期的実行の違いは本質的な問題とはならない。
* LustreとSignal
    * LustreおよびSignalは、リアルタイムシステム向けの宣言的・同期的なデータフロープログラミング言語である。
    * データフロープログラミングと呼ばれるプログラミングパラダイムにおいては、データ間の依存関係を定義することによってプログラムの動作が表現される。
    * FRPはデータフロープログラミングの手法を純粋関数型プログラミングの枠内で実現したものとみなすこともできる。
* Simulink
    * Simulinkはブロック線図を元にして組込みシステムの挙動をシミュレートするツールである。
    * ブロック線図から組込みシステムを動作させるC言語コードを生成することもできる。
    * FRPはブロック線図をプログラムによって表現する手法とみなすこともできるため、Simulinkの手法はFRPの手法に通じるものがある。
    * 微分積分等の物理的な演算を主に行うシステムはSimulinkによる記述がより適しているといえる。
    * 複雑な状態遷移等のソフトウェア的な処理を主に行うシステムの記述はFRPによる記述がより適しているといえる。
* CFRPとJuniper
    * CFRPは小規模組込みシステム向けのFRP言語であり、言語としてはオリジナルのElmのサブセットとなっている。
    * JuniperはArduino向けのFRP言語であり、Elmに近いFRPの手法を採用する非純粋関数型言語となっている。
    * いずれもSFRPと同様、メモリ資源が乏しい環境でFRPを行う試みである。
* Emfrp
    * Emfrpは筆者によるもう１つの小規模組込みシステム向けのFRP言語であり、本論文におけるSFRPの前身となる存在である。
    * SFRPはEmfrpにおける入出力の手法と全体の構文をより洗練させた言語である。
    * 現在のところ、Emfrpには導入されているがSFRPでは未実装となっている言語機能がある。
        * ユニットテストに関するサポートやノードモジュール機能等
    * 現在EmfrpとSFRPは互いに独立したプロジェクトであるが、両者の目的は同一であり、将来的には統合される可能性がある。

---
# 結論
## まとめ
* マイクロコントローラにおいて省メモリに実行することを前提とした、シンプルなFRP言語を考案した。
* このFRP言語のコンパイラを実装し、実行時性能について定量的評価を行った。
* 実例研究として、実際にこのコンパイラを用いて実用的な組込みシステム開発を行った。
* 本言語の特徴的な要素を取り上げ、それらの記述性および実用性への寄与について論じた。

## 今後の課題
* 反復的な処理を簡単に表現するためのサポート
    * 現状では、反復的な処理を言語内で実現するための唯一の手段は、時変値の直前値を介する方法のみ。
    * 末尾再帰関数の定義をサポートすれば、メモリ使用量を一定以下に抑えつつ反復処理を簡単に表現出来る。
    * ただし、時変値演算の即時性が失われるため、リアルタイム性の保証がされなくなる。
        * つまり、Δtが微小であるという保証がされなくなる。
* 無名関数および第一級関数のサポート
    * 非再帰的な無名関数および第一級関数をサポートすることは原理上可能。
* 時変値に関する典型処理を簡単に記述するためのサポート
    * 典型処理の例としては、Bool型時変値のエッジを取るような処理。
    * 関数定義の内部で、初期値付きの時変値を生成することができるようにする。
        * 現状では、初期値付きの時変値を定義できるのはトップレベルのみ。
* 外部定義関数を削減するためのサポート
    * メモリマップドI/Oによる入出力を外部定義を介さずに利用できるような言語機能をサポートする。
* 外部定義関数の利便性を高めるためのサポート
    * 外部定義関数の返り値型として、タプル型に加えてMaybe型をサポートする。
* 組込みシステムの低レベルな要素に対するサポート
    * スリープモードへの移行や内部pollingの頻度に関する制御をサポートする。

---
# 謝辞
* 最後に書く

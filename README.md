# robertpelloni/ksm-v2 — Fork Status (Honest, 2026-11-19)

This is **Robert Pelloni's fork** of K-Shoot MANIA v2. It is not the upstream project.

- **Upstream:** [kshootmania/ksm-v2](https://github.com/kshootmania/ksm-v2) by masaka / m4saka (MIT) — the C++ / OpenSiv3D rewrite of K-Shoot MANIA v1 (HSP). Upstream was at version **2.2.0** (Nov 15, 2026) when this fork was last checked.
- **This branch (`master`) is the newest / canonical branch of the fork:** `VERSION` **2.0.0-alpha33**, head Nov 15, 2026.
- **`main` is stale:** `VERSION` 2.0.0-alpha31, head Nov 7, 2026. The two branches have diverged. Branch unification (Tier 2) has **not** been done — it is Robert's call.
- **What is proven vs. claimed:**
  - The docs in this repo (HANDOFF / DASHBOARD / ROADMAP, Jules-authored) claim the `kson` chart library builds on Linux and `npm test` passes 45 test cases / 979 assertions. That test (`scripts/test_wrapper.sh`) covers the **kson library only** — it is **not Muse-verified**, and it says nothing about the full game building or running.
  - The full GUI game requires Visual Studio 2022 + OpenSiv3D 0.6.16 on Windows. That build is **not verified**.
  - **Internet Ranking:** client-side submission and (on `master`) a mock server receiver exist. A production ranking backend is **not deployed**.
  - **Input Gate:** client logic is implemented with a **mock active** — no live Input Gate service is verified.
  - **Editor:** `kshooteditor` exists as a CMake target / stub only. Parity with the v1 editor has **not** been checked.
- The upstream README is kept below as a record. Its build instructions and contribution policy are upstream's, not this fork's.

---

## Original upstream README (kept as record)

K-Shoot MANIA v2 (v1 Remastered)
========================

## 開発状況

https://github.com/kshootmania/ksm-v2/wiki#%E9%96%8B%E7%99%BA%E7%8A%B6%E6%B3%81

## 開発のコンセプト

- KSM v1(HSP版)をC++で完全再現することを目標
    - 次期バージョンへの足がかりとなるコードベース作成
- 組み直しによる各種改善
    - パフォーマンス改善
    - LASER判定の改善
    - 譜面データのハッシュ値をキーとしたスコア管理・コース作成による利便性向上
- kson形式の実験的導入
    - kson形式を実行中の内部データの構造として使用する実験
    - 追加機能の部分的サポート

## ビルドに必要な環境 / Build Requirements

- Visual Studio 2022
    - Visual Studio Installer上で「C++ によるデスクトップ開発」にチェックを付けてインストールしてください /  
      Install "Desktop development with C++" from the Visual Studio Installer
- OpenSiv3D 0.6.16 (installed to Visual Studio 2022)
    - 下記からインストーラをダウンロードして実行してください / Download and run the installer below
        - https://github.com/Siv3D/OpenSiv3D#downloads

## Contributionについて

本プロジェクトは実装途中の段階であり、満たす必要がある要件(互換性に関する必須事項など)が十分にドキュメント化されていないため、機能実装についてのContributionは当面の間受け付けておりません。

既存処理の不具合修正に関するContributionについてはPull Requestにて歓迎いたします。

## Contributors

@m4saka, @ario1281, @mikomikotaishi

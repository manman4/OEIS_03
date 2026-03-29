def solve_a349404(limit)
  # 階乗をあらかじめ計算しておく (キャッシュ)
  facts = [1]
  i = 1
  while i <= limit
    facts << facts[i - 1] * i
    i += 1
  end

  # 組合せ数 C(n, k) を前計算
  comb = Array.new(limit + 1){Array.new(limit + 1, 0)}
  i = 0
  while i <= limit
    row = comb[i]
    row[0] = 1
    row[i] = 1
    j = 1
    while j < i
      row[j] = comb[i - 1][j - 1] + comb[i - 1][j]
      j += 1
    end
    i += 1
  end

  # n=0 のケース
  puts "0 1"

  n = 1
  while n <= limit
    # 証明（遷移とスケーリングの正当性）:
    # 1) 係数の基本式
    #    展開では各 k=1..n で (x1+...+xk) から 1 つを選ぶ。
    #    多重指数 e1..en (ei>=0, sum ei = n) に対する係数は
    #      coef(e) = n! / ∏(e_i!)
    #    （全 n 回の選択で e_i 回 x_i を選ぶ組合せ数）。
    #
    # 2) 部分和による状態
    #    s_k = e1+...+e_k とし、e_k = s_k - s_{k-1} と書く。
    #    k までの最大係数を C_k(s)（s = s_k）と置く。
    #
    # 3) 遷移式（係数版）の厳密な導出
    #    k-1 までの指数が固定されているとき、残りの (n-(k-1)) 個の括弧
    #    （(x1+...+xk), ..., (x1+...+xn)）のうち、
    #    x_k を選ぶ回数が e_k であるような選び方の数は
    #      comb(s_k-1, s_{k-1}-1) / e_k!
    #    になる。これは次の分割で数えられる:
    #      - k-1 までに選ばれた回数が s_{k-1} なので、
    #        残りの選択の中で「x1..x_{k-1} のどれか」を選ぶ回数は s_{k-1}-1。
    #      - それらを (s_k-1) 回のうちどこに配置するかが comb(s_k-1, s_{k-1}-1)。
    #      - x_k を選ぶ e_k 回の並びは同種なので e_k! で割る。
    #    よって
    #      C_k(s) = max_{prev_s} C_{k-1}(prev_s) * comb(s-1, prev_s-1) / e_k!
    #    となり、ここで comb(s-1, prev_s-1) = (s-1)! / ((prev_s-1)!(s-prev_s)!)
    #    を用いると
    #      comb(s-1, prev_s-1) / e_k! = (s - k + 1) * (prev_s - k + 2) / (s-1)!
    #    から、等価に
    #      C_k(s) = max_{prev_s} C_{k-1}(prev_s) * (prev_s - k + 2) / e_k!
    #    を得る。
    #
    # 4) スケーリングで整数化
    #    D_k(s) = (s-1)! * C_k(s) とおくと
    #      D_k(s) = max_{prev_s} D_{k-1}(prev_s) * (s - k + 1) * C(s-1, prev_s-1)
    #    となり、二項係数 C(.,.) だけで整数計算できる。
    #
    # 5) 初期条件
    #    k=1 では C_1(s)=1/(s-1)! なので D_1(s)=1。
    # dp[s] は D_k[s] = (s-1)! * (最大係数) を保持する整数DP
    # ステップ k=1 の初期化: D_1[s] = 1
    dp = Array.new(n + 1, 0)
    s = 1
    while s <= n
      dp[s] = 1
      s += 1
    end

    # ステップ k=2 から n まで
    k = 2
    new_dp = Array.new(n + 1, 0)
    while k <= n
      new_dp.fill(0)
      # 現在の累積和 s は k 以上 n 以下
      s = k
      while s <= n
        row = comb[s - 1]
        best_val = 0
        # 前のステップの累積和 prev_s は k-1 以上 s 以下
        prev_s = k - 1
        while prev_s <= s
          # D_new = D_prev * (s - k + 1) * C(s-1, prev_s-1)
          current_val = dp[prev_s] * (s - k + 1) * row[prev_s - 1]
          best_val = current_val if current_val > best_val
          prev_s += 1
        end
        new_dp[s] = best_val
        s += 1
      end
      tmp = dp
      dp = new_dp
      new_dp = tmp
      k += 1
    end

    # 最終結果を出力: 最大係数 = D_n[n] / (n-1)!
    a_n = dp[n] / facts[n - 1]
    break if a_n.to_s.size > 1000 # 1000桁を超える場合は終了
    puts "#{n} #{a_n}"
    n += 1
  end
end

# 100まで実行
solve_a349404(600)

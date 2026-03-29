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

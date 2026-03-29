def solve_a349404(limit)
  # 階乗のキャッシュ
  facts = [1]
  i = 1
  while i <= limit
    facts << facts[i - 1] * i
    i += 1
  end

  # 組合せ数 C(n, k) を前計算
  comb = Array.new(limit + 1) { Array.new(limit + 1, 0) }
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

  # n=0 の定義
  puts "0 1"
  return if limit == 0

  n = 1
  while n <= limit
    # dp[s] は「最大係数」を (s-1)! でスケールした整数DP
    # もともとの発想は「係数そのものを追う」こと:
    #   x1 の指数 e1 に対して係数は 1 / e1!
    # ただし高速化のため、(s-1)! を掛けた値 D_k[s] を追跡している
    # よって k=1 の初期値は 1 (＝(s-1)! * 1/e1!)
    dp = Array.new(n + 1, 0)
    s = 1
    while s <= n
      dp[s] = 1
      s += 1
    end

    # k=2 から n まで順番に変数の指数を決定していく
    k = 2
    new_dp = Array.new(n + 1, 0)
    while k <= n
      new_dp.fill(0)
      # パーキング条件: k番目までの累積和 s は k 以上
      s = k
      while s <= n
        row = comb[s - 1]
        best_val = 0
        
        # prev_s (k-1番目までの累積和) からの遷移
        prev_s = k - 1
        while prev_s <= s
          # 漸化式 (整数版):
          # D_new = D_prev * (s - k + 1) * C(s-1, prev_s-1)
          # これは「係数版」の
          #   係数_new = 係数_prev * (prev_s - k + 2) / e_k!
          # と等価 (スケーリング済み)
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

    # 最終的な累積和は必ず n になる
    a_n = dp[n] / facts[n - 1]
    break if a_n.to_s.size > 1000
    puts "#{n} #{a_n}"
    n += 1
  end
end

# 実行
solve_a349404(550)

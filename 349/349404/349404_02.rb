def solve_a349404(limit)
  # 階乗のキャッシュ
  facts = [1]
  (1..limit).each { |i| facts << facts.last * i }

  # 組合せ数 C(n, k) を前計算
  comb = Array.new(limit + 1) { Array.new(limit + 1, 0) }
  (0..limit).each do |i|
    comb[i][0] = 1
    comb[i][i] = 1
    (1...i).each do |j|
      comb[i][j] = comb[i - 1][j - 1] + comb[i - 1][j]
    end
  end

  # n=0 の定義
  puts "0 1"
  return if limit == 0

  (1..limit).each{|n|
    # dp[s] は「最大係数」を (s-1)! でスケールした整数DP
    # もともとの発想は「係数そのものを追う」こと:
    #   x1 の指数 e1 に対して係数は 1 / e1!
    # ただし高速化のため、(s-1)! を掛けた値 D_k[s] を追跡している
    # よって k=1 の初期値は 1 (＝(s-1)! * 1/e1!)
    dp = Array.new(n + 1, 0)
    (1..n).each{|s| dp[s] = 1}

    # k=2 から n まで順番に変数の指数を決定していく
    (2..n).each{|k|
      new_dp = Array.new(n + 1, 0)
      # パーキング条件: k番目までの累積和 s は k 以上
      (k..n).each{|s|
        best_val = 0
        
        # prev_s (k-1番目までの累積和) からの遷移
        (k - 1..s).each{|prev_s|
          # 漸化式 (整数版):
          # D_new = D_prev * (s - k + 1) * C(s-1, prev_s-1)
          # これは「係数版」の
          #   係数_new = 係数_prev * (prev_s - k + 2) / e_k!
          # と等価 (スケーリング済み)
          current_val = dp[prev_s] * (s - k + 1) * comb[s - 1][prev_s - 1]
          best_val = current_val if current_val > best_val
        }
        new_dp[s] = best_val
      }
      dp = new_dp
    }

    # 最終的な累積和は必ず n になる
    puts "#{n} #{dp[n] / facts[n - 1]}"
  }
end

# 実行
solve_a349404(200)

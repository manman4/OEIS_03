def list_a_for_fixed_m(max_n, m)
  # dp[i][k] : 和が i, パーツ数が k の「m+1 以上の相異なるパーツ」の順序なし分割数
  dp = Array.new(max_n + 1){Array.new(max_n + 1, 0)}
  dp[0][0] = 1

  # 事前に階乗を計算（k! と (k+1)! 用）
  fact = [1]
  (1..max_n).each {|i| fact << fact[-1] * i }

  # m+1 以上のパーツを使って DP テーブルを構築
  # このループは 1 回で済む
  ((m + 1)..max_n).each{|val|
    max_n.downto(val).each{|i|
      (1..max_n).each{|k|
        dp[i][k] += dp[i - val][k - 1]
      }
    }
  }

  # 各 n について a(n, m) を抽出
  (0..max_n).each{|n|
    target = n - m
    ans = 0
    # target (n-m) を作るパーツ数 k に対して、(k+1)! を掛ける
    (0..target).each{|k|
      ans += dp[target][k] * fact[k + 1]
    }
    break if ans.to_s.size > 1000
    print n
    print ' '
    puts ans
  }
end

n = 1000
m = 2
list_a_for_fixed_m(n, m)
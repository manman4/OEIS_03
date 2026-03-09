# G.f.: 1 + Sum_{n>0} x^n / Product_{d|n} (1-x^d)
# この数列は A083710 (ある部が他の全ての部を割り切る分割の数) に相当します。

def a(limit)
  # 結果を格納する配列 (x^0 から x^limit まで)
  a = Array.new(limit + 1, 0)
  a[0] = 1 # 定数項

  # 各 n について項 x^n / Product_{d|n} (1-x^d) の係数を計算し加算する
  (1..limit).each{ |n|
    # n の約数を抽出 (これが分割の「部」になる)
    divs = []
    (1..n).each{|d| divs << d if n % d == 0}

    # この項の分母 1 / Product_{d|n} (1-x^d) を DP で展開
    # ターゲットとなる次数は limit - n
    dp_limit = limit - n
    dp = Array.new(dp_limit + 1, 0)
    dp[0] = 1

    divs.each{|d|
      (d..dp_limit).each{ |i|
        dp[i] += dp[i - d]
      }
    }

    # x^k の係数を a[n + k] に加算 (x^n のシフト分)
    (0..dp_limit).each{|k|
      a[n + k] += dp[k]
    }
  }
  a
end

# m>0
def s(f_ary, g_ary, n, m)
  s = 0
  (m..n).each{|i| s += i * f_ary[i] * g_ary[i] ** (n / i) if n % i == 0}
  s
end

def A(ary, n, m = 1)
  a_ary = [1]
  a = [0] + (1..n).map{|i| ary.inject(0){|s, j| s + s(j[0], j[1], i, m)}}
  (1..n).each{|i| a_ary << (1..i).inject(0){|s, j| s + a[j] * a_ary[-j]} / i}
  a_ary
end

def p(n)
  ary1 = Array.new(n + 1, 1)
  # Product_{k>0} 1/(1 - x^k)
  A([[ary1, ary1]], n)
end

n = 10100
m = 10000

a_ary = a(n)
p_ary = p(n)
b_ary = [1] + (1..n).map{|i| p_ary[i] - a_ary[i]}

(0..m).each{|i|
  j = b_ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
require 'set'

def distinct_coeff_count(n, facts, comb, profile = nil)
  return 1 if n == 0

  # dp[s] = set (Hash) of D_k(s) values where D_k(s) = (s-1)! * C_k(s)
  dp = Array.new(n + 1)
  (1..n).each{|s| dp[s] = {1 => true}} # k=1: D_1(s)=1

  k = 2
  while k <= n
    new_dp = Array.new(n + 1)
    prev_list = []
    (k - 1..n).each{|ps|
      prev_set = dp[ps]
      prev_list << ps if !(prev_set.nil? || prev_set.empty?)
    }
    (k..n).each{|s|
      base = s - k + 1
      row = comb[s - 1]
      size_before = 0
      profile && profile.call(:before, k, s, size_before)
      idx = 0
      while idx < prev_list.size && prev_list[idx] <= s
        prev_s = prev_list[idx]
        prev_set = dp[prev_s]
        # D_new = D_prev * (s - k + 1) * C(s-1, prev_s-1)
        factor = base * row[prev_s - 1]
        dest = new_dp[s]
        if dest.nil?
          dest = {}
          new_dp[s] = dest
        end
        prev_set.each{|v, _| dest[v * factor] = true}
        idx += 1
      end
      if profile
        size_after = new_dp[s] ? new_dp[s].size : 0
        profile.call(:after, k, s, size_after)
      end
    }
    dp = new_dp
    k += 1
  end

  # 最終係数は D_n(n) / (n-1)!
  denom = facts[n - 1]
  coeffs = {}
  dpn = dp[n] || {}
  dpn.each{|v, _| coeffs[v / denom] = true}
  coeffs.size
end

def solve(limit, profile = false)
  # factorials and binomials up to limit
  facts = [1]
  (1..limit).each{|i| facts << facts[i - 1] * i}

  comb = Array.new(limit + 1){Array.new(limit + 1, 0)}
  (0..limit).each{|i|
    comb[i][0] = 1
    comb[i][i] = 1
    (1..i - 1).each{|j| comb[i][j] = comb[i - 1][j - 1] + comb[i - 1][j]}
  }

  (0..limit).each{|n|
    if profile
      prof = lambda do |stage, k, s, size|
        if stage == :after
          print "k="
          print k
          print " s="
          print s
          print " size="
          puts size
        end
      end
      a_n = distinct_coeff_count(n, facts, comb, prof)
    else
      a_n = distinct_coeff_count(n, facts, comb)
    end
    puts "#{n} #{a_n}"
  }
end

solve(26, false)

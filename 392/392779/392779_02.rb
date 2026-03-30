def verify_sum_coeff(limit)
  (0..limit).each{|n|
    # brute expand: monomial exponent vector -> coefficient
    dp = {Array.new(n, 0) => 1}
    (1..n).each{|k|
      new_dp = Hash.new(0)
      dp.each{|exp, coef|
        (1..k).each{|i|
          nexp = exp.dup
          nexp[i - 1] += 1
          new_dp[nexp] += coef
        }
      }
      dp = new_dp
    }

    ary = dp.values
    print("#{n} 最大:#{ary.max} 異なる個数:#{ary.uniq.size} 最初の10個:#{ary[0..10]} ")
    sum_coeff = ary.sum
    fact = (1..n).inject(1, :*)
    ok = (sum_coeff == fact)
    puts "sum=#{sum_coeff} n!#{'=' if ok}#{fact} ok=#{ok}"
  }
end

# NOTE: Exponential; keep limit small.
verify_sum_coeff(14)

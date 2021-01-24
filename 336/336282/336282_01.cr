def a336282(n)
  return 1 if n == 0
  cnt = 0
  (0..n - 1).to_a.each_permutation{|h|
    flag = true
    sig = Array.new(n, 0)
    sig[0] = 2
    h[1..-1].each{|j|
      sig[j] = 2
      while j > -1
        j -= 1
        if sig[j] > 0
          sig[j] -= 1
          break
        end
      end
      if j == -1
        flag = false
        break 
      end
    }
    cnt += 1 if flag
  }
  cnt
end

(1..14).each{|i| p [i, a336282(i)]}


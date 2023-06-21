def a(n, r)
  s = 0
  (1..n).each{|i|
    break if i ** (r + 1) > n
    (1..n).each{|j|
      k = i ** (r + 1) + j ** r
      break if k > n
      s += 1 if k == n
    }
  }
  s
end

n = 1073741825
(1..4).each{|r|
  (1..n).each{|i| 
    if a(i, r) == 2
      puts i
      break
    end
  }
}


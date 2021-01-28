def search(cards, n, num)
  if num == 0
    @count += 1
  else
    (2 * n - 1).times{|i|
      if cards[i] == 0
        (i + 1..i + num + 1).each{|j|
          if cards[j] == 0
            cards[i], cards[j] = num, num
            search(cards, n, num - 1)
            cards[i], cards[j] = 0, 0
          end
        }
      end
    }
  end
end

def A(n)
  cards = [0] * n * 2
  @count = 0
  search(cards, n, n)
  @count
end

(1..2).each{|i| p A(i)}

s, t = 0, 0
(1..2).to_a.repeated_permutation(4){|i|
  if i.count(1) == 2 && i.count(2) == 2
    s += 1
    flag = true
    (1..2).each{|k|
      if k < i.rindex(k) - i.index(k) - 1
        flag = false
      end
    }
    if flag
      t += 1
      p [t, i]
    else
     # p i
    end
  end
}
p t


s, t = 0, 0
(1..3).to_a.repeated_permutation(6){|i|
  if i.count(1) == 2 && i.count(2) == 2 && i.count(3) == 2
    s += 1
    flag = true
    (1..3).each{|k|
      if k < i.rindex(k) - i.index(k) - 1
        flag = false
      end
    }
    if flag
      t += 1
      p [t, i]
    else
     # p i
    end
  end
}
p s

s, t = 0, 0
(1..4).to_a.repeated_permutation(8){|i|
  if i.count(1) == 2 && i.count(2) == 2 && i.count(3) == 2 && i.count(4) == 2
    s += 1
    flag = true
    (1..4).each{|k|
      if k < i.rindex(k) - i.index(k) - 1
        flag = false
      end
    }
    if flag
      t += 1
      #p [t, i]
    else
     # p i
    end
  end
}
p t

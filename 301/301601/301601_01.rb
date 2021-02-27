def A(n)
p n

  flag = false
  (1..n).each{|a|
    (a..n).each{|b|
      if a ** 6 + 10 * b ** 6 <= n ** 6
        (b..n).each{|c|
          if a ** 6 + b ** 6 + 9 * c ** 6 <= n ** 6
            (c..n).each{|d|
              if a ** 6 + b ** 6 + c ** 6 + 8 * d ** 6 <= n ** 6
                (d..n).each{|e|
                  if a ** 6 + b ** 6 + c ** 6 + d ** 6 + 7 * e ** 6 <= n ** 6
                  (e..n).each{|f|
                    if a ** 6 + b ** 6 + c ** 6 + d ** 6 + e ** 6 + 6 * f ** 6 <= n ** 6
                      (f..n).each{|g|
                        if a ** 6 + b ** 6 + c ** 6 + d ** 6 + e ** 6 + f ** 6 + 5 * g ** 6 <= n ** 6
                          (g..n).each{|h|
                            if a ** 6 + b ** 6 + c ** 6 + d ** 6 + e ** 6 + f ** 6 + g ** 6 + 4 * h ** 6 <= n ** 6
                              (h..n).each{|i|
                                if a ** 6 + b ** 6 + c ** 6 + d ** 6 + e ** 6 + f ** 6 + g ** 6 + h ** 6 + 3 * i ** 6 <= n ** 6
                                (i..n).each{|j|
                                  if a ** 6 + b ** 6 + c ** 6 + d ** 6 + e ** 6 + f ** 6 + g ** 6 + h ** 6 + i ** 6 + 2 * j ** 6 <= n ** 6
                                  (j..n).each{|k|
                                    if a ** 6 + b ** 6 + c ** 6 + d ** 6 + e ** 6 + f ** 6 + g ** 6 + h ** 6 + i ** 6 + j ** 6 + k ** 6 == n ** 6
                                      p [n, [a, b, c, d, e, f, g, h, i, j, k]]
                                      return true
                                    end
                                    }
                                  end
                                  }
                                end
                              }
                            end
                          }
                        end
                       }
                     end
                   }
                  end
                }
              end
            }
          end
        }
      end
    }
  }
  flag
end

n = 100
p (38..n).select{|i| A(i)}

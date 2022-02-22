(define call/cc
  (lambda (thunk)
    (call/cc% thunk)))

(define call-with-current-continuation call/cc)

(define call-with-values
  (lambda (producer consumer)
    (call-with-values% producer consumer)))

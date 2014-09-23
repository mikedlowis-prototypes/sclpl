(declare (unit server) (uses eval));(uses spiffy intarweb posix))
(require-extension spiffy)

(define index-template
"<!DOCTYPE html PUBLIC
           \"-//W3C//DTD XHTML 1.0 Strict//EN\"
           \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">
<html lang=\"en\">
  <head>
    <title>Index of ~a</title>
  </head>
  <body>
    <div>~a</div>
  </body>
</html>")

(define entry-template "<a href=\"~a\">~a</a><br/>\n")

(define (html-response html)
  (with-headers `((content-type text/html)
                  (content-length ,(string-length html)))
    (lambda ()
      (write-logged-response)
      (display html (response-port (current-response))))))

(define (generate-index path)
  (define curr_root (string-append (root-path) path))
  (define entries (glob (string-append curr_root "/*")))
  (apply string-append
         (map (lambda (e)
                (define pth (if (equal? "/" path) path (string-append path "/" e)))
                (sprintf entry-template pth e))
              (map (lambda (e) (car (reverse (string-split e "/"))))
                   entries))))

(define (index-handler path)
  (html-response (sprintf index-template path (generate-index path))))

(define (start-pkg-server port root)
  (server-port port)
  (root-path root)
  (handle-directory index-handler)
  (start-server))


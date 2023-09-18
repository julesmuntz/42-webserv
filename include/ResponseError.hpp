#define ERROR(status, status_description) ('<!DOCTYPE html> \
<html lang="en"> \
<head> \
    <meta charset="UTF-8"> \
    <meta http-equiv="X-UA-Compatible" content="IE=edge"> \
    <meta name="viewport" content="width=device-width, initial-scale=1.0"> \
    <title>Error</title> \
    <style> \
        body { \
            display: flex; \
            flex-direction: column; \
            justify-content: center; \
            align-items: center; \
            background-image: url(https://media.giphy.com/media/U3qYN8S0j3bpK/giphy.gif); \
            background-color: black; \
        } \
        h1 { \
            color: white; \
            /*border: white solid; \
            border-radius: 20px;*/ \
             text-align: center; \
            font-size: 2.3rem; \
            padding: 20px; \
            font-family: \'Trebuchet MS\', \'Lucida Sans Unicode\', \'Lucida Grande\', \'Lucida Sans\', Arial, sans-serif; \
        } \
        div.tenor-gif-embed { \
            max-width: 40%; \
        } \
    </style> \
</head> \
<body> \
    <h1>Error ' status ' - ' status_description ' :(</h1> \
    <div class="tenor-gif-embed" data-postid="22693394" data-share-method="host" data-aspect-ratio="0.87" data-width="100%"> \
        </div> <script type="text/javascript" async src="https://tenor.com/embed.js"></script> \
</body> \
</html>')

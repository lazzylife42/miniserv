/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   miniserv.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smonte-e <smonte-e@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 15:34:32 by smonte-e          #+#    #+#             */
/*   Updated: 2024/10/02 15:41:54 by smonte-e         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char *response = "Hello from server\n";

    // Créer le socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attacher le socket à l'adresse et au port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Mettre le serveur en mode écoute
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Boucle principale pour accepter plusieurs connexions
    while (1) {
        // Accepter une connexion entrante
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        printf("New client connected...\n");

        // Boucle pour gérer plusieurs messages d'un même client
        while (1) {
            memset(buffer, 0, BUFFER_SIZE); // Réinitialiser le buffer

            // Recevoir un message du client
            int valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
            if (valread < 0) {
                perror("recv failed");
                close(new_socket);
                break; // Sortir de la boucle si erreur
            } else if (valread == 0) {
                printf("Client disconnected.\n");
                close(new_socket);
                break; // Le client a fermé la connexion
            }

            printf("Message received: %s\n", buffer);

            // Envoyer une réponse au client
            send(new_socket, response, strlen(response), 0);
            printf("Response sent: %s\n", response);
        }
    }

    // Fermer le socket du serveur (ce code n'est jamais atteint, mais important de le prévoir)
    close(server_fd);

    return 0;
}
